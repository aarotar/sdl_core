//
// Copyright (c) 2013, Ford Motor Company
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following
// disclaimer in the documentation and/or other materials provided with the
// distribution.
//
// Neither the name of the Ford Motor Company nor the names of its contributors
// may be used to endorse or promote products derived from this software
// without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

#ifndef NSSMARTDEVICELINKRPC_AUDIOCAPTUREQUALITYMARSHALLER_INCLUDE
#define NSSMARTDEVICELINKRPC_AUDIOCAPTUREQUALITYMARSHALLER_INCLUDE

#include <string>
#include <json/json.h>

#include "PerfectHashTable.h"

#include "../include/JSONHandler/SDLRPCObjects/V1/AudioCaptureQuality.h"


/*
  interface	Ford Sync RAPI
  version	1.2
  date		2011-05-17
  generated at	Thu Jan 24 06:36:21 2013
  source stamp	Thu Jan 24 06:35:34 2013
  author	RC
*/

namespace NsSmartDeviceLinkRPC
{

//! marshalling class for AudioCaptureQuality

  class AudioCaptureQualityMarshaller
  {
  public:
  
    static std::string toName(const AudioCaptureQuality& e) 	{ return getName(e.mInternal) ?: ""; }
  
    static bool fromName(AudioCaptureQuality& e,const std::string& s)
    { 
      return (e.mInternal=getIndex(s.c_str()))!=AudioCaptureQuality::INVALID_ENUM;
    }
  
    static bool checkIntegrity(AudioCaptureQuality& e)		{ return e.mInternal!=AudioCaptureQuality::INVALID_ENUM; } 
    static bool checkIntegrityConst(const AudioCaptureQuality& e)	{ return e.mInternal!=AudioCaptureQuality::INVALID_ENUM; } 
  
    static bool fromString(const std::string& s,AudioCaptureQuality& e);
    static const std::string toString(const AudioCaptureQuality& e);
  
    static bool fromJSON(const Json::Value& s,AudioCaptureQuality& e);
    static Json::Value toJSON(const AudioCaptureQuality& e);
  
    static const char* getName(AudioCaptureQuality::AudioCaptureQualityInternal e)
    {
       return (e>=0 && e<2) ? mHashTable[e].name : NULL;
    }
  
    static const AudioCaptureQuality::AudioCaptureQualityInternal getIndex(const char* s);
  
    static const PerfectHashTable mHashTable[2];
  };
  
}

#endif
