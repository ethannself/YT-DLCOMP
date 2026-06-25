# YT-DLCOMP

This program is my attempt to automate the processing of monthly song compilations for a community that I am apart of. 
Community members enter their submission into a google form, which is sent to a sheet for processing.  google form which contain youtube links and timestamps and downloads the videos for the creator to process.


## Steps
1. Create a public google form. Questions on google form can be worded however, but must take the following information in this exact order: Question 1. username Question 2. youtube link Question 3. timestamp
2. Link the form to a google sheet: Go to responses, and press the button to connect to Sheets.
3. Enable Google Forms API on your Google account [here](https://console.cloud.google.com/apis/library/forms.googleapis.com)
4. Create an API Key [here](https://console.cloud.google.com/apis/credentials) with Google Sheets API access and copy it.
5. Run the program, enter your API key in the menu File -> Api Key 
6. (Optional) Set custom destination folder at File -> Set Destination Folder
6. Enter the link to the spreadsheet where your responses are saved
7. Click the Start button