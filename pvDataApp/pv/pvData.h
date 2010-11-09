/* pvData.h */
#include <string>
#include <stdexcept>
#include <map>
#ifndef PVDATA_H
#define PVDATA_H
#include "pvIntrospect.h"
#include "noDefaultMethods.h"
#include "requester.h"
#include "byteBuffer.h"
#include "serialize.h"
namespace epics { namespace pvData { 

    class PVAuxInfo;
    class PostHandler;

    class PVField;
    class PVScalar;
    class PVBoolean;
    class PVByte;
    class PVShort;
    class PVInt;
    class PVLong;
    class PVFloat;
    class PVDouble;
    class PVString;

    class PVScalarArray;
    class PVBooleanArray;
    class PVByteArray;
    class PVShortArray;
    class PVIntArray;
    class PVLongArray;
    class PVFloatArray;
    class PVDoubleArray;
    class PVStringArray;

    class PVStructure;
    class PVStructureArray;

    typedef std::map<String,PVScalar * > PVScalarMap;
    typedef PVScalarMap::const_iterator PVScalarMapIter;
    typedef PVStructure * PVStructurePtr;
    typedef PVStructurePtr* PVStructurePtrArray;
    typedef PVField* PVFieldPtr;
    typedef PVFieldPtr * PVFieldPtrArray;
    typedef bool * BooleanArray;
    typedef int8 * ByteArray;
    typedef int16 * ShortArray;
    typedef int32 * IntArray;
    typedef int64 * LongArray;
    typedef float * FloatArray;
    typedef double * DoubleArray;
    typedef String * StringArray;

    class PVAuxInfo : private NoDefaultMethods {
    public:
        PVAuxInfo(PVField *pvField);
        ~PVAuxInfo();
        static int64 getTotalConstruct();
        static int64 getTotalDestruct();
        PVField * getPVField();
        PVScalar * createInfo(String key,ScalarType scalarType);
        PVScalarMap getInfos();
        PVScalar * getInfo(String key);
        void toString(StringBuilder buf);
        void toString(StringBuilder buf,int indentLevel);
    private:
        static void init();
        class PVAuxInfoPvt *pImpl;
        friend class PVDataCreate;
    };

    class PostHandler {
    public:
        virtual void postPut() = 0;
    };

    class PVField : public Requester, public Serializable ,private NoDefaultMethods{
    public:
        virtual ~PVField();
        static int64 getTotalConstruct();
        static int64 getTotalDestruct();
        String getRequesterName() ;
        virtual void message(String message,MessageType messageType) ;
        virtual void setRequester(Requester *prequester);
        int getFieldOffset() ;
        int getNextFieldOffset() ;
        int getNumberFields() ;
        PVAuxInfo * getPVAuxInfo();
        bool isImmutable() ;
        void setImmutable();
        FieldConstPtr getField() ;
        PVStructure * getParent() ;
        void replacePVField(PVField * newPVField);
        void renameField(String  newName);
        void postPut() ;
        void setPostHandler(PostHandler *postHandler);
        virtual void toString(StringBuilder buf) ;
        virtual void toString(StringBuilder buf,int indentLevel) ;
        virtual bool operator==(PVField &pv) = 0;
        virtual bool operator!=(PVField &pv) = 0;
    protected:
        PVField(PVStructure *parent,FieldConstPtr field);
        void replaceStructure(PVStructure *pvStructure);
    private:
        class PVFieldPvt *pImpl;
        static void computeOffset(PVField *pvField);
        static void computeOffset(PVField *pvField,int offset);
        static void init();
        friend class PVDataCreate;
    };

    class PVScalar : public PVField {
    public:
        virtual ~PVScalar();
        ScalarConstPtr getScalar() ;
    protected:
        PVScalar(PVStructure *parent,ScalarConstPtr scalar);
    };

    class PVArray : public PVField, public SerializableArray {
    public:
        virtual ~PVArray();
        int getLength() ;
        void setLength(int length);
        int getCapacity() ;
        bool isCapacityMutable() ;
        void setCapacityMutable(bool isMutable);
        virtual void setCapacity(int capacity) = 0;
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) = 0;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher) = 0;
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) = 0;
    protected:
        PVArray(PVStructure *parent,FieldConstPtr field);
        void setCapacityLength(int capacity,int length);
    private:
        class PVArrayPvt * pImpl;
    };


    class PVScalarArray : public PVArray {
    public:
        virtual ~PVScalarArray();
        ScalarArrayConstPtr getScalarArray() ;
        virtual void setCapacity(int capacity) = 0;
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) = 0;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl *pflusher) = 0;
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) = 0;
    protected:
        PVScalarArray(PVStructure *parent,ScalarArrayConstPtr scalarArray);
    private:
    };

    class StructureArrayData {
    public:
        PVStructurePtrArray data;
        int offset;
    };

    class PVStructureArray : public PVArray {
    public:
        virtual ~PVStructureArray();
        virtual StructureArrayConstPtr getStructureArray() = 0;
        virtual void setCapacity(int capacity) = 0;
        virtual int get(int offset, int length,
            StructureArrayData *data) = 0;
        virtual int put(int offset,int length,
            PVStructurePtrArray from, int fromOffset) = 0;
        virtual void shareData( PVStructurePtrArray value,int capacity,int length) = 0;
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher) = 0 ;
        virtual void deserialize(ByteBuffer *buffer,
            DeserializableControl *pflusher) = 0;
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) = 0;
    protected:
        PVStructureArray(PVStructure *parent,
            StructureArrayConstPtr structureArray);
    private:
    };
    

    class PVStructure : public PVField,public BitSetSerializable {
    public:
        virtual ~PVStructure();
        StructureConstPtr getStructure();
        PVFieldPtrArray getPVFields();
        PVField *getSubField(String fieldName);
        PVField *getSubField(int fieldOffset);
        void appendPVField(PVField *pvField);
        void appendPVFields(int numberFields,PVFieldPtrArray pvFields);
        void removePVField(String fieldName);
        PVBoolean *getBooleanField(String fieldName);
        PVByte *getByteField(String fieldName);
        PVShort *getShortField(String fieldName);
        PVInt *getIntField(String fieldName);
        PVLong *getLongField(String fieldName);
        PVFloat *getFloatField(String fieldName);
        PVDouble *getDoubleField(String fieldName);
        PVString *getStringField(String fieldName);
        PVStructure *getStructureField(String fieldName);
        PVScalarArray *getScalarArrayField(
            String fieldName,ScalarType elementType);
        PVStructureArray *getStructureArrayField(String fieldName);
        String getExtendsStructureName();
        bool putExtendsStructureName(
            String extendsStructureName);
        virtual bool operator==(PVField &pv) ;
        virtual bool operator!=(PVField &pv) ;
        virtual void serialize(
            ByteBuffer *pbuffer,SerializableControl *pflusher) ;
        virtual void deserialize(
            ByteBuffer *pbuffer,DeserializableControl *pflusher);
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher, int offset, int count) ;
        virtual void serialize(ByteBuffer *pbuffer,
            SerializableControl *pflusher,BitSet *pbitSet) ;
        virtual void deserialize(ByteBuffer *pbuffer,
            DeserializableControl*pflusher,BitSet *pbitSet);
    protected:
        PVStructure(PVStructure *parent,StructureConstPtr structure);
    private:
        class PVStructurePvt * pImpl;
    };

    class PVBoolean : public PVScalar {
    public:
        virtual ~PVBoolean();
        virtual bool get() = 0;
        virtual void put(bool value) = 0;
    protected:
        PVBoolean(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
    };

    class PVByte : public PVScalar {
    public:
        virtual ~PVByte();
        virtual int8 get() = 0;
        virtual void put(int8 value) = 0;
    protected:
        PVByte(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
    };

    class PVShort : public PVScalar {
    public:
        virtual ~PVShort();
        virtual int16 get() = 0;
        virtual void put(int16 value) = 0;
    protected:
        PVShort(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
    };

    class PVInt : public PVScalar{
    public:
        virtual ~PVInt();
        virtual int32 get() = 0;
        virtual void put(int32 value) = 0;
    protected:
        PVInt(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
    };

    class PVLong : public PVScalar {
    public:
        virtual ~PVLong();
        virtual int64 get() = 0;
        virtual void put(int64 value) = 0;
    protected:
        PVLong(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
    };

    class PVFloat : public PVScalar {
    public:
        virtual ~PVFloat();
        virtual float get() = 0;
        virtual void put(float value) = 0;
    protected:
        PVFloat(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
    };

    class PVDouble : public PVScalar {
    public:
        virtual ~PVDouble();
        virtual double get() = 0;
        virtual void put(double value) = 0;
    protected:
        PVDouble(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
    };

    class PVString : public PVScalar {
    public:
        virtual ~PVString();
        virtual String get() = 0;
        virtual void put(String value) = 0;
    protected:
        PVString(PVStructure *parent,ScalarConstPtr scalar)
        : PVScalar(parent,scalar) {}
    private:
    };

    class BooleanArrayData {
    public:
        BooleanArray data;
        int offset;
    };

    class PVBooleanArray : public PVScalarArray {
    public:
        virtual ~PVBooleanArray();
        virtual void setCapacity(int capacity) = 0;
        virtual int get(int offset, int length, BooleanArrayData *data) = 0;
        virtual int put(int offset,int length, BooleanArray from, int fromOffset) = 0;
        virtual void shareData(BooleanArray value,int capacity,int length) = 0;
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) = 0;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher) = 0;
    protected:
        PVBooleanArray(PVStructure *parent,ScalarArrayConstPtr scalar);
    private:
    };


    class ByteArrayData {
    public:
        ByteArray data;
        int offset;
    };

    class PVByteArray : public PVScalarArray {
    public:
        virtual ~PVByteArray();
        virtual void setCapacity(int capacity) = 0;
        virtual int get(int offset, int length, ByteArrayData *data) = 0;
        virtual int put(int offset,int length, ByteArray  from, int fromOffset) = 0;
        virtual void shareData(ByteArray value,int capacity,int length) = 0;
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) = 0;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher) = 0;
    protected:
        PVByteArray(PVStructure *parent,ScalarArrayConstPtr scalar);
    private:
    };

    class ShortArrayData {
    public:
        ShortArray data;
        int offset;
    };

    class PVShortArray : public PVScalarArray {
    public:
        virtual ~PVShortArray();
        virtual void setCapacity(int capacity) = 0;
        virtual int get(int offset, int length, ShortArrayData *data) = 0;
        virtual int put(int offset,int length, ShortArray  from, int fromOffset) = 0;
        virtual void shareData(ShortArray value,int capacity,int length) = 0;
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) = 0;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher) = 0;
    protected:
        PVShortArray(PVStructure *parent,ScalarArrayConstPtr scalar);
    private:
    };

    class IntArrayData {
    public:
        IntArray data;
        int offset;
    };

    class PVIntArray : public PVScalarArray {
    public:
        virtual ~PVIntArray();
        virtual void setCapacity(int capacity) = 0;
        virtual int get(int offset, int length, IntArrayData *data) = 0;
        virtual int put(int offset,int length, IntArray  from, int fromOffset)= 0;
        virtual void shareData(IntArray value,int capacity,int length)= 0;
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) = 0;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher)= 0;
    protected:
        PVIntArray(PVStructure *parent,ScalarArrayConstPtr scalar);
    private:
    };

    class LongArrayData {
    public:
        LongArray data;
        int offset;
    };

    class PVLongArray : public PVScalarArray {
    public:
        virtual ~PVLongArray();
        virtual void setCapacity(int capacity) = 0;
        virtual int get(int offset, int length, LongArrayData *data) = 0;
        virtual int put(int offset,int length, LongArray  from, int fromOffset)= 0;
        virtual void shareData(LongArray value,int capacity,int length)= 0;
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) = 0;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher)= 0;
    protected:
        PVLongArray(PVStructure *parent,ScalarArrayConstPtr scalar);
    private:
    };


    class FloatArrayData {
    public:
        FloatArray data;
        int offset;
    };

    class PVFloatArray : public PVScalarArray {
    public:
        virtual ~PVFloatArray();
        virtual void setCapacity(int capacity) = 0;
        virtual int get(int offset, int length, FloatArrayData *data) = 0;
        virtual int put(int offset,int length, FloatArray  from, int fromOffset)= 0;
        virtual void shareData(FloatArray value,int capacity,int length)= 0;
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) = 0;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher)= 0;
    protected:
        PVFloatArray(PVStructure *parent,ScalarArrayConstPtr scalar);
    private:
    };


    class DoubleArrayData {
    public:
        DoubleArrayData(){}
        ~DoubleArrayData(){};
        DoubleArray data;
        int offset;
    };

    class PVDoubleArray : public PVScalarArray {
    public:
        virtual ~PVDoubleArray();
        virtual void setCapacity(int capacity) = 0;
        virtual int get(int offset, int length, DoubleArrayData *data) = 0;
        virtual int put(int offset,int length, DoubleArray  from, int fromOffset) = 0;
        virtual void shareData(DoubleArray value,int capacity,int length) = 0;
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) = 0;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher) = 0;
    protected:
        PVDoubleArray(PVStructure *parent,ScalarArrayConstPtr scalar);
    private:
    };


    class StringArrayData {
    public:
        StringArray data;
        int offset;
    };

    class PVStringArray : public PVScalarArray {
    public:
        virtual ~PVStringArray();
        virtual void setCapacity(int capacity) = 0;
        virtual int get(int offset, int length, StringArrayData *data) = 0;
        virtual int put(int offset,int length, StringArray  from, int fromOffset)= 0;
        virtual void shareData(StringArray value,int capacity,int length)= 0;
        virtual void serialize(ByteBuffer *pbuffer,SerializableControl *pflusher) = 0;
        virtual void deserialize(ByteBuffer *pbuffer,DeserializableControl *pflusher)= 0;
    protected:
        PVStringArray(PVStructure *parent,ScalarArrayConstPtr scalar);
    private:
    };

    class PVDataCreate {
    public:
       PVField *createPVField(PVStructure *parent,
           FieldConstPtr field);
       PVField *createPVField(PVStructure *parent,
           String fieldName,PVField * fieldToClone);
       PVScalar *createPVScalar(PVStructure *parent,ScalarConstPtr scalar);
       PVScalar *createPVScalar(PVStructure *parent,
           String fieldName,ScalarType scalarType);
       PVScalar *createPVScalar(PVStructure *parent,
           String fieldName,PVScalar * scalarToClone);
       PVScalarArray *createPVScalarArray(PVStructure *parent,
           ScalarArrayConstPtr scalarArray);
       PVScalarArray *createPVScalarArray(PVStructure *parent,
           String fieldName,ScalarType elementType);
       PVScalarArray *createPVScalarArray(PVStructure *parent,
           String fieldName,PVScalarArray * scalarArrayToClone);
       PVStructureArray *createPVStructureArray(PVStructure *parent,
           StructureArrayConstPtr structureArray);
       PVStructure *createPVStructure(PVStructure *parent,
           StructureConstPtr structure);
       PVStructure *createPVStructure(PVStructure *parent,
           String fieldName,int numberFields,FieldConstPtrArray fields);
       PVStructure *createPVStructure(PVStructure *parent,
           String fieldName,PVStructure *structToClone);
    protected:
       PVDataCreate();
       friend PVDataCreate * getPVDataCreate();
    };

    extern PVDataCreate * getPVDataCreate();
    
}}
#endif  /* PVDATA_H */
