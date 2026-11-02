#include "Serializer.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

void Serializer::SerializeObject(UObject* Object, TArray<uint8>& Storage)
{
	FMemoryWriter Writer(Storage, true);
	FObjectAndNameAsStringProxyArchive Ar(Writer, false);
	Ar.ArIsSaveGame = true;
	Ar.ArNoDelta = true;
	Object->Serialize(Ar);
}

void Serializer::DeserializeObject(UObject* Object, const TArray<uint8>& Storage)
{
	FMemoryReader Reader(Storage, true);
	FObjectAndNameAsStringProxyArchive Ar(Reader, false);
	Ar.ArIsSaveGame = true;
	Object->Serialize(Ar);
}