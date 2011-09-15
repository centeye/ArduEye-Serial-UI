/*
  Datasets.h - Definitions for dataset processing from Centeye Vision Sensors
 Centeye, Inc
 Created by Alison Leonard. August, 2011

 ===============================================================================
 Copyright (c) 2011, Centeye, Inc.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of Centeye, Inc. nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL CENTEYE, INC. BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ===============================================================================
*/
#ifndef DATASETS_H
#define DATASETS_H

// max allowable bytes in serial buffer
#define MAX_PACKET_SIZE 1010

// special bytes - communications flags
#define ESC_CHAR    38
#define START_PCKT  90
#define END_PCKT    91
#define END_FRAME 92

// flow control byte definitions
#define GO_CHAR 36
#define CMD_ACK 37
#define ACK_CMD 34

// null values
#define NULL_PROCESS -1
#define RECEIVE_PROCESS 1

// flags for receiving data
#define PCKT_HEADER 2
#define PCKT_DATA 1

// display types
#define DISPLAY_NONE  0
#define DISPLAY_GRAYSCALE_IMAGE 1
#define DISPLAY_CHARTS 2
#define DISPLAY_TEXT 4
#define DISPLAY_DUMP 5
#define DISPLAY_POINTS 6

// Datasets Structure hold Dataset incoming bytes and keeps track of size information
struct DataSets{

public:
    QString name; // english name for dataset (defined in the parameter txt file)
    int DSID; // data set ID (These are defined in the parameter txt file)
    int length; // current length of data set, as opposed to max length
    int width, height;  // current width, height of dataset, these are set in the dataset header
    int SizeFactor; //Dataset is stored in chars, but actual value may be shorts, ints (Size factor 2, 4 respectively)
    char * DataArray; //DataArray stores data received for a particular DSID
    int MaxSize; //max size for dataset - this will be the max size of the DataArrayBuffer (Defined in parameter txt file)
    int DisplayType; // defines how data should be displayed, display type is set in the dataset header
    bool DataReceived; // set to true when datareceived and cleared after it is displayed

    // Constructor
    DataSets()
    {
        SizeFactor = 1;
        DisplayType = DISPLAY_NONE;
        DataReceived = false;
        width = height  = 0;
    }

    // initialize DataSets entry using data from the parameter txt file
    void InitDataArray(int MaxLength)
    {
        // init DataArray for storing incoming data
        DataArray = new char[MaxLength*SizeFactor];
        for(int i = 0; i < MaxLength; i++)
            DataArray[i] = 0;
        // set MaxSize to check for oveflow when data is coming in
        MaxSize = MaxLength*SizeFactor;
        // length is initialized to MaxLength but is updated each time data is received
        length = MaxLength;        
    }

    // clear DataArray
    void Clear()
    {
        for(int i = 0; i < length; i++)
            DataArray[i] = 0;
    }

    // delete must be called in the destructor of the class containing the DataSets struct
    void Delete()
    {
        delete DataArray;
    }
};



#endif // DATASETS_H
