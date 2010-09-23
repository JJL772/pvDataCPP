/*AbstractPVScalar.h*/
#ifndef ABSTRACTPVSCALAR_H
#define ABSTRACTPVSCALAR_H
#include <cstddef>
#include <cstdlib>
#include <string>
#include <cstdio>
#include "pvData.h"
#include "factory.h"

namespace epics { namespace pvData {

    static PVDataCreate *pvDataCreate =0;

    class PVAuxInfoPvt {
    public:
        PVAuxInfoPvt(PVField *pvField)
        :  pvField(pvField),
           theMap(std::map<StringConstPtr, PVScalar * >())
        {} 
        PVField *pvField;
        std::map<StringConstPtr, PVScalar * > theMap;
    };

    PVAuxInfo::PVAuxInfo(PVField *pvField)
    : pImpl(new PVAuxInfoPvt(pvField))
    { }

    PVAuxInfo::~PVAuxInfo() { delete pImpl;}
    
    PVField * PVAuxInfo::getPVField() {
        return pImpl->pvField;
    }

    typedef std::map<StringConstPtr,PVScalar * >::const_iterator map_iterator;

    PVScalar * PVAuxInfo::createInfo(StringConstPtr key,ScalarType scalarType)
    {
        map_iterator i = pImpl->theMap.find(key);
        if(i!=pImpl->theMap.end()) {
            std::string message("AuxoInfo:create key ");
            message += key->c_str();
            message += " already exists with scalarType ";
            ScalarTypeFunc::toString(&message,scalarType);
            pImpl->pvField->message(&message,errorMessage);
        }
        if(pvDataCreate==0) pvDataCreate = getPVDataCreate();
        PVScalar *pvScalar = pvDataCreate->createPVScalar(0,key,scalarType);
        pImpl->theMap.insert(std::pair<StringConstPtr,PVScalar * >(key, pvScalar));
        return pvScalar;

    }

    std::map<StringConstPtr, PVScalar * > *PVAuxInfo::getInfos()
    {
        return &pImpl->theMap;
    }

    PVScalar * PVAuxInfo::getInfo(StringConstPtr key)
    {
        map_iterator i = pImpl->theMap.find(key);
        if(i!=pImpl->theMap.end()) return i->second;
        return 0;
    }

    void PVAuxInfo::toString(StringPtr buf)
    {
        return PVAuxInfo::toString(buf,0);
    }

    void PVAuxInfo::toString(StringPtr buf,int indentLevel)
    {
        map_iterator i = pImpl->theMap.begin();
        while(i!=pImpl->theMap.end()) {
             StringConstPtr key = i->first;
             PVScalar *value = i->second;
             *buf += " ";
             *buf += key->c_str();
             *buf += " ";
             value->toString(buf);
        }
    }
}}
#endif  /* ABSTRACTPVSCALAR_H */
