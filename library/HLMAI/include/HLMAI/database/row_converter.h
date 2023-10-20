#ifndef ROW_CONVERTER_H_HL_MAI
#define ROW_CONVERTER_H_HL_MAI

#include <Poco/JSON/Object.h>

template <class Derived, class Tuple>
class RowConverter {
  public:
    Poco::JSON::Object::Ptr RowToJSON(const Tuple& row);
    Tuple RowFromJSON(const Poco::JSON::Object::Ptr& jsonObject);
};

template <class Derived, class Tuple>
Poco::JSON::Object::Ptr 
RowConverter<Derived, Tuple>::RowToJSON(const Tuple& row) {
  Poco::JSON::Object::Ptr json_object = new Poco::JSON::Object();
  static_cast<Derived*>(this)->ToJSON(json_object, row);
  return json_object;
}

template <class Derived, class Tuple>
Tuple RowConverter<Derived, Tuple>::RowFromJSON(
    const Poco::JSON::Object::Ptr& jsonObject) {
  Tuple row;
  static_cast<Derived*>(this)->FromJSON(row, jsonObject);
  return row;
}

#endif