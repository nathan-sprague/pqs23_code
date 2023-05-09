



void init_fs() {
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    fs_working = false;
  } else {
    Serial.println("SPIFF Mount Successful");
    fs_working = true;
  }
}


void saveData(String fileToOpen, uint8_t writeData[], uint16_t bufferSize) {
  /*
    Write binary data to the file system. Verify the file is open and write to it
  */

  File currentFile = MAIN_FS.open(fileToOpen, FILE_APPEND);


  Serial.print("writing to : " + fileToOpen);
  for (int i = 0; i < bufferSize; i++) {
    Serial.print(writeData[i]);
    Serial.print(", ");
  }
  Serial.println("");

  currentFile.write(writeData, bufferSize);

}

void deleteFile( const char * path) {
  Serial.printf("Deleting file: %s\n", path);
  if (MAIN_FS.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}
