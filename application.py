from flask import Flask, jsonify, render_template, has_request_context, request, url_for, redirect
import os
import numpy as np
from azure.storage.blob import BlobClient
from base64 import decodestring
from datetime import datetime
from PIL import Image
import io

from azure.cognitiveservices.vision.customvision.prediction import CustomVisionPredictionClient
from msrest.authentication import ApiKeyCredentials

app = Flask(__name__)

# Blob storage
connection_str = "DefaultEndpointsProtocol=..."
blob_name = ""
blob = BlobClient.from_connection_string(conn_str=connection_str, container_name="container1", blob_name=blob_name)

# Custom vision
prediction_key = "..."
ENDPOINT = "https://southcentralus.api.cog..."

credentials = ApiKeyCredentials(in_headers={"Prediction-key": prediction_key})
predictor = CustomVisionPredictionClient(endpoint=ENDPOINT, credentials=credentials)


N = 4 # number of image parts, had to send the image through multiple parts because of the size limits in IoT hub
width, height = 640, 480

correct_part_order = [i for i in range(N)]

message = "default"

@app.route('/update_message', methods=["POST"])
def update_message():

    if not blob.exists(): 
        print("no new data")

    else: # download new from blob storage

        os.remove("./blob_input.json") # removes previous download

        with open("./blob_input.json", "wb") as my_blob: # downloads new one
            blob_data = blob.download_blob()
            print(blob_data)
            blob_data.readinto(my_blob)

        # logic with parts
        last_n_parts = blob_data[-N:] # choosing last N entries in blob representing N parts of the image

        part_order = [part["part"] for part in last_n_parts]

        if part_order == correct_part_order:
            whole_image_str = ""
            
            for part in last_n_parts: whole_image_str += part["data"] # concatenates the entries into one string in base64

            whole_image_str = str.encode(whole_image_str)

            image = Image.fromstring('greyscale', (width,height), decodestring(whole_image_str)) # decodes the string and translates it into PIL object
            image.save("image.png") # saves the image and rewrites the last one
            
            img = Image.open("image.png", 'r')

            img_byte_arr = io.BytesIO()
            img.save(img_byte_arr, format='PNG')

            results = predictor.classify_image("b32c631a-d22b-4254-abe4-fb8b2b98331d", 'Iteration1', img_byte_arr.getvalue()) # calls the custom vision API
            
            message = ""
            for prediction in results.predictions:
                message += prediction.tag_name + ": {0:.2f}%      ".format

            message += f"new message arrived at {datetime.now()}"
            
            return jsonify('', render_template('message_model.html', message=message)) # renders the image in template

        else:
            print("waiting for all parts to arrive")
        
        

@app.route("/", methods=['GET', 'POST'])
def login():
    return render_template("index.html", message=message)


if __name__ == '__main__': 
          app.run() 