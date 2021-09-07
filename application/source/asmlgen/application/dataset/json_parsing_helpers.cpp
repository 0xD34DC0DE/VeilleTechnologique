#include "asmlgen/application/dataset/json_parsing_helpers.h"

#include "rapidjson/error/en.h"

#include <iostream>

namespace dataset
{

// TODO replace assert with debug only assert

rapidjson::Document* GetParsedDocument(const std::vector<char>& json)
{
  if (json.empty()) { return nullptr; }

  auto* document = new rapidjson::Document;

  if (document->Parse(json.data()).HasParseError())
  {
    std::cerr << "Json document parsing error: \n"
              << "Error(offset " << static_cast<unsigned>(document->GetErrorOffset()) << "):\n\t"
              << GetParseError_En(document->GetParseError()) << std::endl;
    return nullptr;
  }

  return document;
}

template<>
uint64_t ParseValueField<uint64_t>(const rapidjson::Value& jsonValueRef, const char* fieldName)
{
  assert(jsonValueRef.HasMember(fieldName));
  return jsonValueRef[fieldName].GetInt64();
}

template<>
uint32_t ParseValueField<uint32_t>(const rapidjson::Value& jsonValueRef, const char* fieldName)
{
  assert(jsonValueRef.HasMember(fieldName));
  return jsonValueRef[fieldName].GetInt();
}

template<>
float ParseValueField<float>(const rapidjson::Value& jsonValueRef, const char* fieldName)
{
  assert(jsonValueRef.HasMember(fieldName));
  return jsonValueRef[fieldName].GetFloat();
}

template<>
double ParseValueField<double>(const rapidjson::Value& jsonValueRef, const char* fieldName)
{
  assert(jsonValueRef.HasMember(fieldName));
  return jsonValueRef[fieldName].GetDouble();
}

template<>
std::string ParseValueField<std::string>(const rapidjson::Value& jsonValueRef, const char* fieldName)
{
  assert(jsonValueRef.HasMember(fieldName));
  return jsonValueRef[fieldName].GetString();
}

} // namespace dataset