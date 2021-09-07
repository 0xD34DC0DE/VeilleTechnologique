#ifndef ASMLGEN_JSON_PARSING_HELPERS_H
#define ASMLGEN_JSON_PARSING_HELPERS_H

#include <rapidjson/document.h>
#include <string>
#include <vector>

namespace dataset
{
// todo add no discard

rapidjson::Document* GetParsedDocument(const std::vector<char>& json);

template<typename T>
T ParseValueField(const rapidjson::Value& jsonValueRef, const char* fieldName);

template<>
uint64_t ParseValueField<uint64_t>(const rapidjson::Value& jsonValueRef, const char* fieldName);

template<>
uint32_t ParseValueField<uint32_t>(const rapidjson::Value& jsonValueRef, const char* fieldName);

template<>
float ParseValueField<float>(const rapidjson::Value& jsonValueRef, const char* fieldName);

template<>
double ParseValueField<double>(const rapidjson::Value& jsonValueRef, const char* fieldName);

template<>
std::string ParseValueField<std::string>(const rapidjson::Value& jsonValueRef, const char* fieldName);

} // namespace dataset

#endif // ASMLGEN_JSON_PARSING_HELPERS_H
