#include "asmlgen/application/dataset/json_parsing_helpers.h"

namespace dataset
{

// TODO replace assert with debug only assert

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