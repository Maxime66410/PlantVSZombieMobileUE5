
#include "Firestore/FieldValue.h"
#include "Firestore/DocumentReference.h"

#if WITH_FIREBASE_FIRESTORE
	THIRD_PARTY_INCLUDES_START
#		include "firebase/variant.h"
#		include "firebase/firestore/geo_point.h"
#		include "firebase/firestore/document_reference.h"
#		include "firebase/firestore/timestamp.h"
	THIRD_PARTY_INCLUDES_END
#endif // WITH_FIREBASE_FIRESTORE

#include "Json.h"


FFirestoreGeoPoint::FFirestoreGeoPoint()
	: Latitude(0.f)
	, Longitude(0.f)
{
}

FFirestoreGeoPoint::FFirestoreGeoPoint(float InLatitude, float InLongitude)
	: Latitude(InLatitude)
	, Longitude(InLongitude)
{
}

FFirestoreTimestamp::FFirestoreTimestamp()
	: Seconds(0)
	, Nanoseconds(0)
{
}

FFirestoreTimestamp::FFirestoreTimestamp(int64 InSeconds, int32 InNanoseconds)
	: Seconds(FMath::Max(0, InSeconds))
	, Nanoseconds(FMath::Max(0, InNanoseconds))
{
}

FFirestoreTimestamp FFirestoreTimestamp::Now()
{
#if WITH_FIREBASE_FIRESTORE
	auto Time = firebase::Timestamp::Now();
	return FFirestoreTimestamp(Time.seconds(), Time.nanoseconds() % 1000000000);
#else
	return FFirestoreTimestamp(0, 0);
#endif
}

FDateTime FFirestoreTimestamp::ToDateTime() const
{
	return FDateTime::FromUnixTimestamp(Seconds);
}

FString FFirestoreTimestamp::ToString() const
{
	return FString::Printf(TEXT("%s:%d"), *ToDateTime().ToString(), Nanoseconds);
}

FFirestoreFieldValue::FFirestoreFieldValue()
#if WITH_FIREBASE_FIRESTORE
	: FieldValue(MakeUnique<firebase::firestore::FieldValue>())
#endif // WITH_FIREBASE_FIRESTORE
{
}
FFirestoreFieldValue::FFirestoreFieldValue(const FFirestoreFieldValue& Other) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	*this = Other;
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue& FFirestoreFieldValue::operator=(const FFirestoreFieldValue& Other)
{
#if WITH_FIREBASE_FIRESTORE
	if (Other.FieldValue)
	{
		*FieldValue.Get() = *Other.FieldValue.Get();
	}
#endif // WITH_FIREBASE_FIRESTORE
	return *this;
}

FFirestoreFieldValue::~FFirestoreFieldValue()
{
}

FFirestoreFieldValue::operator TArray<uint8>() const
{
#if WITH_FIREBASE_FIRESTORE
	if (!FieldValue)
	{
		return {};
	}

	if (FieldValue->is_blob())
	{
		return TArray<uint8>(FieldValue->blob_value(), FieldValue->blob_size());
	}

	if (FieldValue->is_string())
	{
		const auto StringData = FieldValue->string_value();

		return TArray<uint8>(reinterpret_cast<const uint8*>(StringData.c_str()), StringData.size());
	}

	if (FieldValue->is_array())
	{
		const auto Array = FieldValue->array_value();
		
		TArray<uint8> Data;
		Data.Reserve(Array.size());
		
		for (const auto& Element : Array)
		{
			if (Element.is_double())
			{
				Data.Add(static_cast<uint8>(Element.double_value()));
			}
			else if (Element.is_integer())
			{
				Data.Add(static_cast<uint8>(Element.integer_value()));
			}
			else if (Element.is_boolean())
			{
				Data.Add(static_cast<uint8>(Element.boolean_value()));
			}
		}

		return Data;
	}

	UE_LOG(LogFirestore, Error, TEXT("Failed to convert FieldValue to binary. Type didn't match: %d"), FieldValue->type());

	return {};
#else
	return	TArray<uint8>();
#endif // WITH_FIREBASE_FIRESTORE

}

FFirestoreFieldValue::operator TArray<FFirestoreFieldValue>() const
{
	TArray<FFirestoreFieldValue> Array;

#if WITH_FIREBASE_FIRESTORE
	if (FieldValue && FieldValue->is_array())
	{
		const auto& ArrayRaw = FieldValue->array_value();
		Array.Reserve(ArrayRaw.size());

		for (const auto& ArrayElem : ArrayRaw)
		{
			Array.Emplace(ArrayElem);
		}
	}
#endif // WITH_FIREBASE_FIRESTORE

	return Array;
}

FFirestoreFieldValue::operator TMap<FString, FFirestoreFieldValue>() const
{
	TMap<FString, FFirestoreFieldValue> Map;

#if WITH_FIREBASE_FIRESTORE
	if (FieldValue && FieldValue->is_map())
	{
		const auto& MapRaw = FieldValue->map_value();
		Map.Reserve(MapRaw.size());

		for (const auto& MapElem : MapRaw)
		{
			Map.Emplace(UTF8_TO_TCHAR(MapElem.first.c_str()), MapElem.second);
		}
	}
#endif // WITH_FIREBASE_FIRESTORE

	return Map;
}

FFirestoreFieldValue::operator UFirestoreDocumentReference* () const
{
#if WITH_FIREBASE_FIRESTORE
	if (!FieldValue || !FieldValue->is_reference())
	{
		return nullptr;
	}

	UFirestoreDocumentReference* const Reference = NewObject<UFirestoreDocumentReference>();

	*Reference->Reference = FieldValue->reference_value();

	return Reference;
#else
	return nullptr;
#endif // WITH_FIREBASE_FIRESTORE
}

#if WITH_FIREBASE_FIRESTORE
FFirestoreFieldValue::FFirestoreFieldValue(const firebase::firestore::FieldValue& Value) : FFirestoreFieldValue()
{
	*FieldValue = Value;
}
#endif // WITH_FIREBASE_FIRESTORE

FFirestoreFieldValue::FFirestoreFieldValue(const bool bValue) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::Boolean(bValue));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(const int32  Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::Integer((int64_t)Value));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(const int64  Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::Integer(Value));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(const float  Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::Double((float)Value));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(const double Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::Double(Value));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(const TCHAR* Value) : FFirestoreFieldValue(FString(Value))
{
}

FFirestoreFieldValue::FFirestoreFieldValue(const FString& Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::String(TCHAR_TO_UTF8(*Value)));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(const TArray<uint8>& Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::Blob(Value.GetData(), Value.Num()));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(const TArray<FFirestoreFieldValue>& Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	std::vector<firebase::firestore::FieldValue> Values;

	Values.reserve(Value.Num());

	for (const auto& Val : Value)
	{
		Values.push_back(Val);
	}

	SetValue(firebase::firestore::FieldValue::Array(MoveTemp(Values)));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(const FFirestoreGeoPoint& Value)
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::GeoPoint(firebase::firestore::GeoPoint(Value.Latitude, Value.Longitude)));
#endif
}

FFirestoreFieldValue::FFirestoreFieldValue(const FFirestoreTimestamp& Value)
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::Timestamp(firebase::Timestamp(Value.Seconds, Value.Nanoseconds)));
#endif
}

FFirestoreFieldValue::FFirestoreFieldValue(const TMap<FString, FFirestoreFieldValue>& Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	std::unordered_map<std::string, firebase::firestore::FieldValue> Values;

	Values.reserve(Value.Num());

	for (const auto& Val : Value)
	{
		Values.emplace(TCHAR_TO_UTF8(*Val.Key), Val.Value);
	}

	SetValue(firebase::firestore::FieldValue::Map(Values));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(UFirestoreDocumentReference* const Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	if (Value)
	{
		SetValue(firebase::firestore::FieldValue::Reference(*Value->GetInternal()));
	}
#endif // WITH_FIREBASE_FIRESTORE
}

#if WITH_FIREBASE_FIRESTORE
void FFirestoreFieldValue::SetValue(const firebase::firestore::FieldValue& Value)
{
	if (!FieldValue)
	{
		FieldValue = MakeUnique<firebase::firestore::FieldValue>(Value);
	}
	else
	{
		*FieldValue.Get() = Value;
	}
}
#endif // WITH_FIREBASE_FIRESTORE

EFirestoreFieldValueType FFirestoreFieldValue::GetType() const
{
#if WITH_FIREBASE_FIRESTORE
	return (EFirestoreFieldValueType)FieldValue->type();
#else
	return EFirestoreFieldValueType::Null;
#endif
}

#if WITH_FIREBASE_FIRESTORE
#	define RETURN_TYPE(TypeName, DefaultValue) \
		return FieldValue->is_ ## TypeName () ? FieldValue-> TypeName ## _value() : DefaultValue
#else
#	define RETURN_TYPE(TypeName, DefaultValue) return DefaultValue
#endif  // WITH_FIREBASE_FIRESTORE

FFirestoreFieldValue::operator int32() const
{
	return static_cast<int32>((int64)*this);
}

FFirestoreFieldValue::operator int64() const
{
#if WITH_FIREBASE_FIRESTORE
	if (FieldValue && FieldValue->is_double())
	{
		return static_cast<int64>(FieldValue->double_value());
	}
#endif
	RETURN_TYPE(integer, 0);
}

FFirestoreFieldValue::operator bool() const
{
	RETURN_TYPE(boolean, false);
}

FFirestoreFieldValue::operator float() const
{
	return static_cast<float>((double)*this);
}

FFirestoreFieldValue::operator double() const
{
#if WITH_FIREBASE_FIRESTORE
	if (FieldValue && FieldValue->is_integer())
	{
		return static_cast<double>(FieldValue->integer_value());
	}
#endif
	RETURN_TYPE(double, 0.);
}

#undef RETURN_TYPE

FFirestoreFieldValue::operator FFirestoreGeoPoint() const
{
	FFirestoreGeoPoint GeoPoint;

#if WITH_FIREBASE_FIRESTORE
	if (FieldValue && FieldValue->is_geo_point())
	{
		auto Point = FieldValue->geo_point_value();

		GeoPoint.Latitude  = (float)Point.latitude();
		GeoPoint.Longitude = (float)Point.longitude();
	}
	else
	{
		UE_LOG(LogFirestore, Warning, TEXT("Tried to get a field value as Geo Point but it isn't one."));
	}
#endif

	return GeoPoint;
}

FFirestoreFieldValue::operator FFirestoreTimestamp() const
{
	FFirestoreTimestamp Timestamp;

#if WITH_FIREBASE_FIRESTORE
	if (FieldValue && FieldValue->is_timestamp())
	{
		auto Time = FieldValue->timestamp_value();

		Timestamp.Seconds     = Time.seconds();
		Timestamp.Nanoseconds = Time.nanoseconds();
	}
#endif

	return Timestamp;
}

FFirestoreFieldValue::operator FString() const
{
#if WITH_FIREBASE_FIRESTORE
	if (!FieldValue)
	{
		return {};
	}

	if (FieldValue->is_string())
	{
		return UTF8_TO_TCHAR(FieldValue->string_value().c_str());
	}

	if (FieldValue->is_double())
	{
		return FString::SanitizeFloat(FieldValue->double_value());
	}

	if (FieldValue->is_integer())
	{
		return FString::Printf(TEXT("%ld"), FieldValue->integer_value());
	}

	if (FieldValue->is_boolean())
	{
		return FieldValue->boolean_value() ? TEXT("True") : TEXT("False");
	}

	if (FieldValue->is_map())
	{
		return TEXT("[Object object]");
	}

	if (FieldValue->is_array())
	{
		return TEXT("[Array array]");
	}

	if (FieldValue->is_null())
	{
		return TEXT("null");
	}

	if (FieldValue->is_blob())
	{
		return TEXT("[Binary]");
	}

	if (FieldValue->is_timestamp())
	{
		auto Timestamp = FieldValue->timestamp_value();
		return FString::Printf(TEXT("%ld:%d"), Timestamp.seconds(), Timestamp.nanoseconds());
	}

	if (FieldValue->is_geo_point())
	{
		auto GeoPoint = FieldValue->geo_point_value();
		return FString::Printf(TEXT("%f,%f"), GeoPoint.longitude(), GeoPoint.latitude());
	}

	if (FieldValue->is_reference())
	{
		return TEXT("[Ref reference]");
	}

	return {};
#else
	return TEXT("FIRESTORE_DISABLED");
#endif
}

bool FFirestoreFieldValue::ToBool() const
{
	return *this;
}

double FFirestoreFieldValue::ToDouble() const
{
	return *this;
}

float FFirestoreFieldValue::ToFloat() const
{
	return *this;
}

int64 FFirestoreFieldValue::ToInt64() const
{
	return *this;
}

int32 FFirestoreFieldValue::ToInt32() const
{
	return *this;
}

FString FFirestoreFieldValue::ToString() const
{
	return *this;
}

TArray<uint8> FFirestoreFieldValue::ToBinary() const
{
	return *this;
}

UFirestoreDocumentReference* FFirestoreFieldValue::ToDocumentReference() const
{
	return *this;
}

TMap<FString, FFirestoreFieldValue> FFirestoreFieldValue::ToMap() const
{
	return *this;
}

TArray<FFirestoreFieldValue> FFirestoreFieldValue::ToArray() const
{
	return *this;
}

FFirestoreGeoPoint FFirestoreFieldValue::ToGeoPoint() const
{
	return *this;
}

FFirestoreTimestamp FFirestoreFieldValue::ToTimestamp() const
{
	return *this;
}

bool    FFirestoreFieldValue::IsNull()   const
{
#if WITH_FIREBASE_FIRESTORE
	return !FieldValue || FieldValue->is_null();
#else 
	return FFirestoreFieldValue();
#endif
}

FFirestoreFieldValue FFirestoreFieldValue::Delete()
{
#if WITH_FIREBASE_FIRESTORE
	return firebase::firestore::FieldValue::Delete();
#else 
	return FFirestoreFieldValue();
#endif
}

FFirestoreFieldValue FFirestoreFieldValue::ServerTimestamp()
{
#if WITH_FIREBASE_FIRESTORE
	return firebase::firestore::FieldValue::ServerTimestamp();
#else 
	return FFirestoreFieldValue();
#endif
}

FFirestoreFieldValue FFirestoreFieldValue::Null()
{
#if WITH_FIREBASE_FIRESTORE
	return firebase::firestore::FieldValue::Null();
#else 
	return FFirestoreFieldValue();
#endif
}

FFirestoreFieldValue FFirestoreFieldValue::ArrayUnion(const TArray<FFirestoreFieldValue>& Elements)
{
#if WITH_FIREBASE_FIRESTORE
	std::vector<firebase::firestore::FieldValue> elements;

	elements.reserve(Elements.Num());

	for (const FFirestoreFieldValue& Value : Elements)
	{
		elements.push_back(*Value.FieldValue);
	}

	return firebase::firestore::FieldValue::ArrayUnion(MoveTemp(elements));
#else 
	return FFirestoreFieldValue();
#endif
}

FFirestoreFieldValue FFirestoreFieldValue::ArrayRemove(const TArray<FFirestoreFieldValue>& Elements)
{
#if WITH_FIREBASE_FIRESTORE
	std::vector<firebase::firestore::FieldValue> elements;

	elements.reserve(Elements.Num());

	for (const FFirestoreFieldValue& Value : Elements)
	{
		elements.push_back(*Value.FieldValue);
	}

	return firebase::firestore::FieldValue::ArrayRemove(MoveTemp(elements));
#else 
	return FFirestoreFieldValue();
#endif
}

#if WITH_FIREBASE_FIRESTORE
static FFirestoreFieldValue FromJSON(const TSharedPtr<FJsonValue>& Value)
{
	switch (Value->Type)
	{
	case EJson::Boolean:	return Value->AsBool();
	case EJson::Null:		return FFirestoreFieldValue::Null();
	case EJson::Number:		return Value->AsNumber();
	case EJson::String:		return Value->AsString();
	case EJson::Object:
	{
		const auto& Object = Value->AsObject();

		TMap<FString, FFirestoreFieldValue> InnerObject;
		InnerObject.Reserve(Object->Values.Num());

		for (const auto& Field : Object->Values)
		{
			InnerObject.Emplace(Field.Key, FromJSON(Field.Value));
		}

		return FFirestoreFieldValue(MoveTemp(InnerObject));

	} break;
	case EJson::Array:
	{
		const auto& Array = Value->AsArray();
		
		TArray<FFirestoreFieldValue> InnerArray;
		InnerArray.Reserve(Array.Num());

		for (const auto& Element : Array)
		{
			InnerArray.Emplace(FromJSON(Element));
		}

		return FFirestoreFieldValue(MoveTemp(InnerArray));

	} break;
	}

	UE_LOG(LogFirestore, Error, TEXT("Unknown JSON type %d."), Value->Type);

	return {};
}
#endif

FFirestoreFieldValue FFirestoreFieldValue::FromJSON(FString JSON)
{
#if WITH_FIREBASE_FIRESTORE
	TSharedPtr<FJsonValue> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JSON);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed))
	{
		return ::FromJSON(JsonParsed);
	}
	else
	{
		UE_LOG(LogFirestore, Error, TEXT("Failed to parse JSON."));
	}
#endif
	return {};
}

#if WITH_FIREBASE_FIRESTORE
static FString ToJSON(const firebase::firestore::FieldValue& FieldValue)
{
#if WITH_FIREBASE_FIRESTORE
	using FFieldValueType = firebase::firestore::FieldValue::Type;

	switch (FieldValue.type())
	{
		// Primitive types
	case FFieldValueType::kBoolean:		return FieldValue.boolean_value() ? TEXT("true") : TEXT("false");
	case FFieldValueType::kDouble:		return FString::SanitizeFloat(FieldValue.double_value());
	case FFieldValueType::kInteger:		return FString::Printf(TEXT("%lld"), FieldValue.integer_value());
	case FFieldValueType::kNull:		return TEXT("null");
	case FFieldValueType::kString:		return FString::Printf(TEXT("\"%s\""), UTF8_TO_TCHAR(FieldValue.string_value().c_str()));
	case FFieldValueType::kTimestamp:	return FString::Printf(TEXT("\"%s\""), UTF8_TO_TCHAR(FieldValue.timestamp_value().ToString().c_str()));
	case FFieldValueType::kReference:	return TEXT("{\"$reference\":true}"); // No support for references.
	case FFieldValueType::kBlob:		return TEXT("\"[BLOB DATA]\""); // No support for blob.
	case FFieldValueType::kGeoPoint:	return FString::Printf(TEXT("\"%s\""), FieldValue.geo_point_value().ToString().c_str());

		// Sentinels.
	case FFieldValueType::kServerTimestamp:	return TEXT("{\"$senti_type\":\"server_timestamp\"}");
	case FFieldValueType::kArrayRemove:		return TEXT("{\"$senti_type\":\"arr_remove\"}");
	case FFieldValueType::kArrayUnion:		return TEXT("{\"$senti_type\":\"arr_union\"}");
	case FFieldValueType::kDelete:			return TEXT("{\"$senti_type\":\"op_delete\"}");
	case FFieldValueType::kIncrementDouble:	return TEXT("{\"$senti_type\":\"inc_double\"}");
	case FFieldValueType::kIncrementInteger:return TEXT("{\"$senti_type\":\"inc_int\"}");

		// Compound objects.
	case FFieldValueType::kArray:
	{
		const auto ArrayData = FieldValue.array_value();

		TStringBuilder<1024> Data;

		Data += TEXT("[");
		
		if (ArrayData.size() > 0)
		{
			Data += ::ToJSON(ArrayData[0]);
		}

		for (size_t i = 1; i < ArrayData.size(); ++i)
		{
			Data += TEXT(",");
			Data += ::ToJSON(ArrayData[i]);
		}

		Data += TEXT("]");

		return Data.ToString();
	}
	case FFieldValueType::kMap:
	{
		TStringBuilder<1024> Json;

		const auto MapData = FieldValue.map_value();

		if (MapData.size() > 0)
		{
			Json += TEXT("{");

			for (const auto& Element : MapData)
			{
				Json += TEXT("\"");
				Json += UTF8_TO_TCHAR(Element.first.c_str());
				Json += TEXT("\":");
				Json += *::ToJSON(Element.second);
			}

			Json.RemoveAt(Json.Len() - 1, 1);
			Json += TEXT('}');

			return Json.ToString();
		}
		
		return TEXT("{}");
	}
	} 
#endif
	return {};
}
#endif

FString FFirestoreFieldValue::ToJSON() const
{
#if WITH_FIREBASE_FIRESTORE
	return FieldValue ? ::ToJSON(*FieldValue) : TEXT("null");
#else
	return {};
#endif
}

