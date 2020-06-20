### UObject (Unreal Objects)

The UObject, coupled with UClass, provides:

+ Reflection of properties and methods
+ Serialization of properties
+ Garbage collection
+ Finding a UObject by name
+ Configurable values for properties.
+ Networking support for properties and methods.

**UClass** describes what an instance of a UObject will look like, what properties are available for serialization, networking and so on.

### Actor

An actor is any object that can be placed into level. Actors do not directly store transform data. The transform data of the actor's Root Component is used. Actors are the containers that hold special types of Object called Components. The **BeginPlay, Tick, EndPlay** are the events called in Actor's lifecycles.

##### UStruct

Like pod of the c++ class. Not garbage collected. Not need inherit.

##### Object/Actor Iterators

```c++
for (TObjectIterator<UObject> It; It; ++It)
{
    UObject* CurrentObject = *It;
}

// get objects only derive from that class
// This gets actor that only from the "World"
for(TObjectIterator<AEnemy> It(World); It;++i)
{}
```

##### Garbage Collection

In the garbage collector, the **root set** is a list of objects that the collector knowns will never be garbage collected. An object will not be garbage collected as long as there is a path of references from an object in the root set to the object in question. If no such path to the root set, the object is called **unreachable** and will be collected the next time the garbage collectors runs. **Any UObject pointer stored in a UPROPERTY or in a UE4 container class (such as TArray) is considered a "reference" for the purpose of garbage collection.**

```c++
UCLASS()
class AMyActor : public AActor
{
    GENERATED_BODY()

public:
    UPROPERTY()
    MyGCType* SafeObject;

    MyGCType* DoomedObject;

    AMyActor(const FObjectInitializer& ObjectInitializer)
        : Super(ObjectInitializer)
    {
        SafeObject = NewObject<MyGCType>();
        DoomedObject = NewObject<MyGCType>();
    }
};

void SpawnMyActor(UWorld* World, FVector Location, FRotator Rotation)
{
    World->SpawnActor<AMyActor>(Location, Rotation);
}
```

In the above code, the DoomedObject will be garbage collected because it is not store in PROPERTY or in a template class.

##### Non-UObject References (manually add reference)

For normal c++ objects, override **FGCObject's** **AddReferenceObejcts**

```c++
class FMyNormalClass : public FGCObject
{
public:
    UObject* SafeObject;

    FMyNormalClass(UObject* Object)
        : SafeObject(Object)
    {
    }
	
    // manually add reference here.
    void AddReferencedObjects(FReferenceCollector& Collector) override
    {
        Collector.AddReferencedObject(SafeObject);
    }
};
```

When a UObject is garbage collected, all UPROPERTY reference to it will be set to null. Check methods

```c++
if(MyActor->safeObject != nullptr)
{
    // do something.
}
```

Actors have has **Destroy** called on them are not removed until the garbage collector runs. Check the **IsPenddingKill** to see if a UObject is awaiting for its deletion.

### Strings

#### Encoding

All strings  in the UE4 are stored in memory in UTF-16 format as FStrings or TCHAR arrays. Most code assumes 2 bytes in one codepoint so only the Basic Multilingual Plane is supported so Unreal's internal encoding is more  correctly described as UCS-2. 

When serializing the packages to/from disk or during networking, strings with all TCHAR characters less than 0xff are stored as a series of 8-bit bytes, and otherwise as 2-byte UTF-16 strings. The serialization code can deal with any endian conversion as necessary.

##### Text Files loaded by UE4

When loading, UE4 recognizes the Unicode byte-order-mark in a UTF-16 file if present. If not, it depends on the platform.

##### Transform functions between encodings

+ TCHAR_TO_ANSI: converts an engine string (TCHAR*) to an ANSI one.
+ TCHAR_TO_OEM
+ ANSI_TO_TCHAR
+ TCHAR_TO_UTF8: converts an ANSI string to TCHAR*.
+ UTF8_TO_TCHAR

The objects these macros declare have very short lifetimes. They are meant to be used as parameters to functions. When go out of the scope, the string will be released.

#### FName

FName provide a very lightweight system for using strings, where a given string is stored only in a data table, even if it is reused. FName is immutable, and cannot be manipulated. It is fast to look up and access FNames with keys. Another feature of the FName subsystem is the use of a hash table to provide fast string to FName conversions.

FNames are case-insensitive, and are stored as a combination of an index into a table of unique strings and an instance number.

Fnames can only be converted to FStrings and FText, and can only be converted from FStrings.

```c++
FName a = FName(TEXT("This is My testFName")); 		// creattion of FName;
String a = a.ToString();   // FName to FString;
FText b = FText::FromName(a);  // FName to FText;

FString strA = TEXT("aaaa");
fnameA = FName(*strA);   // from FString to FName;
FRotator rotPelvis = Mesh->MeshGetInstance(this))->GetBoneRotation(FName(TEXT("pelvis")));  		// using of FName
```

**The name of the bone gets loaded into FName table when the package loads, so the constructor for the FName finds the bone's name in the hash table, avoiding an allocation**

Comparisons of the Fnames campares the values in index, which is significant CPU saving.

If you want to determin if an FName is in the name table, but you do not want to automatically add it, you can supply a different search type to the FName constructor:

```c++
if(FName(TEXT("pelvis"), FName_Find) != Name_None)
{
	// do something
}
```

#### FText:

​	pass for temp

#### FString:

##### Conversion

FString is the only string class that allows for manipulation (including case changing, excerpting substring, search, modify).

```c++
FString a = FString(TEXT("This is a str"));
// convert to FName
FName b = FName(*a);
// convert to FText
FText c = FText::FromString(a);

// Fname to FString
a = b.ToString();
// FText to FString
a = c.ToString();
//  onvert to bool, int, float
a.ToBool();
FCString::Atoi(*a);
FCString::Atof(*a);
```

##### Comparisons

The overloaded == operator can be used to compare two FStrings, or FString and an array of TCHAR*s. **FString::Equals()** method with **ESearchCase::IngoreCase** and **ESearchCase::CaseSensitive** to compare FStrings for case sensitivity. Default ignore case.

```c++
TestString.Equals(TEXT("Test"), ESearchCase::CaseSensitive);
```

##### Searching

+ **FString::Contains()**: Searches FString or TCHAR* subString. return true or false. The ESearchCase is used to specify case sensitity. The **ESeachDir** specifies the search direction.

  ```
  TestString.Contains(TEXT("Test"), ESearchCase::CaseSensitity, ESearchDir::FromEnd);
  ```

+ **FString::Find**: return the index of the first found instance of substring (FString or TCHAR*). Returns -1 if not found. Accept ESearchCase and ESearchDir.

#### Building FStrings

+ **Concatenation**: += and +.  Only takes FStrings as arguments.

+ **Printf**: Like c++ printf. When using %s parameter to include FStrings, the * operator must be used to return the TCHAR* required for the %s parameter.3iokkkkkllllllll

  ```c++
  const FString testString = FString::Printf(TEXT("%d: %d"), numA, numB);
  ```

#### FStrings in HUDs:

##### Print to Viewport: Use the AddOnScreenDebugMessage

```c++
// first parametr is key to prevent the same message from being ad multiple times. The time, color, and debugmessage.
GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blur, testFString);
```

##### Print to Output Log

```c++
UE_LOG(LogClass, Log, TEXT("This is testing:%s"), *testString);
```

+ LogClass is the log category.
+ Log is the verbosity level to use. This is defined in **ELogVerBosity** enum which are **Fatal, Error, Warning,  Display, Log, Verbose, or VeryVErbose**.
+ string to display.

#### Encoding Conversion Macros

The FString class is built upon a TArray of TCHARs.

+ TCHAR_TO_ANSI: Converts the engine string (TCHAR*) to an ANSI one.
+ ANSI_TO_TCHAR: Converts the ANSI string to an engine string (TCHAR*);

The object those macros declare have very short lifetimes. They are meant to used as parameters to functions. You can not assign a variable to the contents of the converted string as the object will go out of the scope and the the string will be released.

### TArray

Most like vector. TArray is a value type, meaning that it should be treated similarly as any other built-in types (It is not designed to be extended, and creating or destroying TArray instances with new and delete is not a recommended practice).  Its elements is also value type. Destruction of a TArray will result in the destruction of any elements it contains. Creating a TArray variable from another will copy its elements into the new variable.

##### Add and Emplace:

The Add create a temporary FString from the string literal and then move the contents of that temporary FString into a new FString inside the container, whereas Emplace will just create the new FString directly using the string literal. Emplace avoids creating a temporary variable.

In general, Emplace is preferable to Add, in that it avoids creating unnecessary temporary variables as the call site which are then copied or moved into the container. **Use Add for trivial types and Emplace otherwise.**

```c++
TArray<FString> sArray;
sArray.Init(10, 5);   // init with five ten values in it.

sArray.add(TEXT("Hello"));
sArray.Emplace(TEXT("World"));
sArray.Append(otherArray);
sArray.AddUnique(TEXT("HHA"));
sArray.Insert(content, pos);
// set the number of array elements, if not enough, auto create, if more, remove the redundant elements.
sArray.SetNum(6);

// iteration for range, and index ignroed here
// iterator: TArray.CreateIteator or TArray.CreateConstIterator
for(aut It = StrArr.CreateConstIterator(); It; ++It)
{
    std::cout<< *It<<std::endl;
}
// lambdas can be offered.
sArray.Sort;
sArray.HeapSort;
sArray.StableSort;

// pointer to the elements in the array, mainly for C-style API
sArray.GetData();
// index from backward
sArray.Last(3);
// check existance
sArray.Contains;
sArray.ContainsByPredicate;        // specify look rules.
// find index
sArray.Find;
sArray.FindLast;
```

### TMap

A collection of key-value pairs, similar to std::map. You can use any type for the key, as long as it has a **GetTypeHash** function defined for it.

### TSet

TSet has faster implementations of these operators and protects against adding non-unique elements automatically. 

### Container Iterators

```c++
for(auto itr = container.CreateIterator(); itr; ++itr)
{
    // or CreatConstItertor;
    auto ele = *itr;  // get the element;
    
    // iterator operators
    itr.RemoveCurrent(); // for TSets and TMaps
    // move operator
    ++itr;
    --itr;
    itr += offset;
    
    // reset to the first element
    itr.Reset();
}
```

### Container hash functions

TSet and TMap requires the use of hash functions internally. This function takes a const pointer or reference to your type and returns uint32;

```c++
uint32 GetTypeHash(const MyClass& instance);
```

