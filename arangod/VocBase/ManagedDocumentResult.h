////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_VOC_BASE_MANAGED_DOCUMENT_RESULT_H
#define ARANGOD_VOC_BASE_MANAGED_DOCUMENT_RESULT_H 1

#include "Basics/Common.h"
#include "VocBase/LocalDocumentId.h"
#include "VocBase/voc-types.h"

namespace arangodb {
namespace velocypack {
class Builder;
}

namespace aql {
struct AqlValue;
}

class ManagedDocumentResult {
 public:
  ManagedDocumentResult() :
    _length(0),
    _localDocumentId(),
    _vpack(nullptr),
    _managed(false),
    _useString(false) {}
  ~ManagedDocumentResult() { reset(); }
  ManagedDocumentResult(ManagedDocumentResult const& other) = delete;
  ManagedDocumentResult& operator=(ManagedDocumentResult const& other) = delete;

  ManagedDocumentResult& operator=(ManagedDocumentResult&& other) {
    if (other._useString) {
      setManaged(std::move(other._string), other._localDocumentId);
      other._managed = false;
      other.reset();
    } else if (other._managed){
      reset();
      _vpack = other._vpack;
      _length = other._length;
      _localDocumentId = other._localDocumentId;
      _managed = true;
      other._managed = false;
      other.reset();
    } else {
      setUnmanaged(other._vpack, other._localDocumentId);
    }
    return *this;
  }

  ManagedDocumentResult(ManagedDocumentResult&& other) = delete;

  void clone(ManagedDocumentResult& cloned) const;

  //add unmanaged vpack 
  void setUnmanaged(uint8_t const* vpack, LocalDocumentId const& documentId);

  void setManaged(uint8_t const* vpack, LocalDocumentId const& documentId);
  void setManaged(std::string&& str, LocalDocumentId const& documentId);

  inline LocalDocumentId localDocumentId() const { return _localDocumentId; }
  
  void reset() noexcept;

  std::string* prepareStringUsage() { 
    reset();
    _useString = true;
    return &_string; 
  }
  
  void setManagedAfterStringUsage(LocalDocumentId const& documentId);
  
  inline uint8_t const* vpack() const {
    TRI_ASSERT(_vpack != nullptr);
    return _vpack;
  }
  
  inline bool empty() const { return _vpack == nullptr; }

  inline bool canUseInExternal() const {
    return (!_managed && !_useString);
  }
  
  void addToBuilder(velocypack::Builder& builder, bool allowExternals) const;

 private:
  uint64_t _length;
  LocalDocumentId _localDocumentId;
  uint8_t* _vpack;
  std::string _string;
  bool _managed;
  bool _useString;
};

}

#endif
