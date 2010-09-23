/* pvDataMain.cpp */
/* Author:  Marty Kraimer Date:    17MAR2000 */

#include <stddef.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "pvData.h"

using namespace epics::pvData;

static FieldCreate * pfieldCreate = 0;
static PVDataCreate *pvDataCreate = 0;
static std::string theBuffer("");
static std::string *buffer = &theBuffer;

void testDouble() {
    printf("\ntestDouble\n");
    std::string valueName("value");
    ScalarConstPtr pscalar = pfieldCreate->createScalar(&valueName,pvDouble);
    PVScalar *pvScalar = pvDataCreate->createPVScalar(0,pscalar);
    PVDouble *pvValue = (PVDouble *)pvScalar;
    double value = 2;
    pvValue->put(value);
    double getValue = pvValue->get();
    if(value!=getValue) {
        fprintf(stderr,"ERROR getValue put %f get %f\n",value,getValue);
    }
    buffer->clear();
    *buffer += "value ";
    pvValue->toString(buffer);
    printf("%s\n",buffer->c_str());
    int offset = pvValue->getFieldOffset();
    int nextOffset = pvValue->getNextFieldOffset();
    int numberFields = pvValue->getNumberFields();
    PVAuxInfo *auxInfo = pvValue->getPVAuxInfo();
    epicsBoolean isImmutable = pvValue->isImmutable();
    PVStructure *pvParent = pvValue->getParent();
    printf("offset %d nextOffset %d numberFields %d auxInfo %p immutable %s parent %p\n",
        offset,nextOffset,numberFields,auxInfo,
        ((isImmutable==epicsFalse) ? "false" : "true"),
        pvParent);
    FieldConstPtr field = pvValue->getField();
    buffer->clear();
    *buffer += "field ";
    field->toString(buffer);
    printf("%s\n",buffer->c_str());
    ScalarConstPtr scalar = dynamic_cast<ScalarConstPtr>(field);
    if(scalar!=field) {
        fprintf(stderr,"ERROR field!=scalar field %p scalar %p\n",field,scalar);
    }
    delete pvValue;
}

int main(int argc,char *argv[])
{
    pfieldCreate = getFieldCreate();
    pvDataCreate = getPVDataCreate();
    testDouble();
    return(0);
}
