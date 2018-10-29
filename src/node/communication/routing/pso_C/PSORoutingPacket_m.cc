//
// Generated file, do not edit! Created by nedtool 4.6 from src/node/communication/routing/pso_C/PSORoutingPacket.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "PSORoutingPacket_m.h"

USING_NAMESPACE


// Another default rule (prevents compiler from choosing base class' doPacking())
template<typename T>
void doPacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doPacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}

template<typename T>
void doUnpacking(cCommBuffer *, T& t) {
    throw cRuntimeError("Parsim error: no doUnpacking() function for type %s or its base class (check .msg and _m.cc/h files!)",opp_typename(typeid(t)));
}




// Template rule for outputting std::vector<T> types
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

nodeData::nodeData()
{
    id = 0;
    serialNo = 0;
    energy = 0;
}

void doPacking(cCommBuffer *b, nodeData& a)
{
    doPacking(b,a.id);
    doPacking(b,a.serialNo);
    doPacking(b,a.energy);
}

void doUnpacking(cCommBuffer *b, nodeData& a)
{
    doUnpacking(b,a.id);
    doUnpacking(b,a.serialNo);
    doUnpacking(b,a.energy);
}

class nodeDataDescriptor : public cClassDescriptor
{
  public:
    nodeDataDescriptor();
    virtual ~nodeDataDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(nodeDataDescriptor);

nodeDataDescriptor::nodeDataDescriptor() : cClassDescriptor("nodeData", "")
{
}

nodeDataDescriptor::~nodeDataDescriptor()
{
}

bool nodeDataDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<nodeData *>(obj)!=NULL;
}

const char *nodeDataDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int nodeDataDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 3+basedesc->getFieldCount(object) : 3;
}

unsigned int nodeDataDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<3) ? fieldTypeFlags[field] : 0;
}

const char *nodeDataDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "id",
        "serialNo",
        "energy",
    };
    return (field>=0 && field<3) ? fieldNames[field] : NULL;
}

int nodeDataDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='i' && strcmp(fieldName, "id")==0) return base+0;
    if (fieldName[0]=='s' && strcmp(fieldName, "serialNo")==0) return base+1;
    if (fieldName[0]=='e' && strcmp(fieldName, "energy")==0) return base+2;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *nodeDataDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "int",
        "int",
        "double",
    };
    return (field>=0 && field<3) ? fieldTypeStrings[field] : NULL;
}

const char *nodeDataDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    }
}

int nodeDataDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    nodeData *pp = (nodeData *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

std::string nodeDataDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    nodeData *pp = (nodeData *)object; (void)pp;
    switch (field) {
        case 0: return long2string(pp->id);
        case 1: return long2string(pp->serialNo);
        case 2: return double2string(pp->energy);
        default: return "";
    }
}

bool nodeDataDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    nodeData *pp = (nodeData *)object; (void)pp;
    switch (field) {
        case 0: pp->id = string2long(value); return true;
        case 1: pp->serialNo = string2long(value); return true;
        case 2: pp->energy = string2double(value); return true;
        default: return false;
    }
}

const char *nodeDataDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        default: return NULL;
    };
}

void *nodeDataDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    nodeData *pp = (nodeData *)object; (void)pp;
    switch (field) {
        default: return NULL;
    }
}

EXECUTE_ON_STARTUP(
    cEnum *e = cEnum::find("PSORoutingPacket_Type");
    if (!e) enums.getInstance()->add(e = new cEnum("PSORoutingPacket_Type"));
    e->insert(PSO_ROUTING_ADV_PACKET, "PSO_ROUTING_ADV_PACKET");
    e->insert(PSO_ROUTING_JOIN_PACKET, "PSO_ROUTING_JOIN_PACKET");
    e->insert(PSO_ROUTING_TDMA_PACKET, "PSO_ROUTING_TDMA_PACKET");
    e->insert(PSO_ROUTING_DATA_PACKET, "PSO_ROUTING_DATA_PACKET");
    e->insert(PSO_ROUTING_FIND_NEIGHBOURS_PACKET, "PSO_ROUTING_FIND_NEIGHBOURS_PACKET");
);

Register_Class(PSORoutingPacket);

PSORoutingPacket::PSORoutingPacket(const char *name, int kind) : ::RoutingPacket(name,kind)
{
    this->numOfPackets_var = 0;
    this->PSORoutingPacketKind_var = 0;
    schedule_arraysize = 0;
    this->schedule_var = 0;
}

PSORoutingPacket::PSORoutingPacket(const PSORoutingPacket& other) : ::RoutingPacket(other)
{
    schedule_arraysize = 0;
    this->schedule_var = 0;
    copy(other);
}

PSORoutingPacket::~PSORoutingPacket()
{
    delete [] schedule_var;
}

PSORoutingPacket& PSORoutingPacket::operator=(const PSORoutingPacket& other)
{
    if (this==&other) return *this;
    ::RoutingPacket::operator=(other);
    copy(other);
    return *this;
}

void PSORoutingPacket::copy(const PSORoutingPacket& other)
{
    this->info_var = other.info_var;
    this->numOfPackets_var = other.numOfPackets_var;
    this->PSORoutingPacketKind_var = other.PSORoutingPacketKind_var;
    delete [] this->schedule_var;
    this->schedule_var = (other.schedule_arraysize==0) ? NULL : new int[other.schedule_arraysize];
    schedule_arraysize = other.schedule_arraysize;
    for (unsigned int i=0; i<schedule_arraysize; i++)
        this->schedule_var[i] = other.schedule_var[i];
}

void PSORoutingPacket::parsimPack(cCommBuffer *b)
{
    ::RoutingPacket::parsimPack(b);
    doPacking(b,this->info_var);
    doPacking(b,this->numOfPackets_var);
    doPacking(b,this->PSORoutingPacketKind_var);
    b->pack(schedule_arraysize);
    doPacking(b,this->schedule_var,schedule_arraysize);
}

void PSORoutingPacket::parsimUnpack(cCommBuffer *b)
{
    ::RoutingPacket::parsimUnpack(b);
    doUnpacking(b,this->info_var);
    doUnpacking(b,this->numOfPackets_var);
    doUnpacking(b,this->PSORoutingPacketKind_var);
    delete [] this->schedule_var;
    b->unpack(schedule_arraysize);
    if (schedule_arraysize==0) {
        this->schedule_var = 0;
    } else {
        this->schedule_var = new int[schedule_arraysize];
        doUnpacking(b,this->schedule_var,schedule_arraysize);
    }
}

nodeData& PSORoutingPacket::getInfo()
{
    return info_var;
}

void PSORoutingPacket::setInfo(const nodeData& info)
{
    this->info_var = info;
}

int PSORoutingPacket::getNumOfPackets() const
{
    return numOfPackets_var;
}

void PSORoutingPacket::setNumOfPackets(int numOfPackets)
{
    this->numOfPackets_var = numOfPackets;
}

int PSORoutingPacket::getPSORoutingPacketKind() const
{
    return PSORoutingPacketKind_var;
}

void PSORoutingPacket::setPSORoutingPacketKind(int PSORoutingPacketKind)
{
    this->PSORoutingPacketKind_var = PSORoutingPacketKind;
}

void PSORoutingPacket::setScheduleArraySize(unsigned int size)
{
    int *schedule_var2 = (size==0) ? NULL : new int[size];
    unsigned int sz = schedule_arraysize < size ? schedule_arraysize : size;
    for (unsigned int i=0; i<sz; i++)
        schedule_var2[i] = this->schedule_var[i];
    for (unsigned int i=sz; i<size; i++)
        schedule_var2[i] = 0;
    schedule_arraysize = size;
    delete [] this->schedule_var;
    this->schedule_var = schedule_var2;
}

unsigned int PSORoutingPacket::getScheduleArraySize() const
{
    return schedule_arraysize;
}

int PSORoutingPacket::getSchedule(unsigned int k) const
{
    if (k>=schedule_arraysize) throw cRuntimeError("Array of size %d indexed by %d", schedule_arraysize, k);
    return schedule_var[k];
}

void PSORoutingPacket::setSchedule(unsigned int k, int schedule)
{
    if (k>=schedule_arraysize) throw cRuntimeError("Array of size %d indexed by %d", schedule_arraysize, k);
    this->schedule_var[k] = schedule;
}

class PSORoutingPacketDescriptor : public cClassDescriptor
{
  public:
    PSORoutingPacketDescriptor();
    virtual ~PSORoutingPacketDescriptor();

    virtual bool doesSupport(cObject *obj) const;
    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual int findField(void *object, const char *fieldName) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual std::string getFieldAsString(void *object, int field, int i) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(PSORoutingPacketDescriptor);

PSORoutingPacketDescriptor::PSORoutingPacketDescriptor() : cClassDescriptor("PSORoutingPacket", "RoutingPacket")
{
}

PSORoutingPacketDescriptor::~PSORoutingPacketDescriptor()
{
}

bool PSORoutingPacketDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<PSORoutingPacket *>(obj)!=NULL;
}

const char *PSORoutingPacketDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int PSORoutingPacketDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int PSORoutingPacketDescriptor::getFieldTypeFlags(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeFlags(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISARRAY | FD_ISEDITABLE,
    };
    return (field>=0 && field<4) ? fieldTypeFlags[field] : 0;
}

const char *PSORoutingPacketDescriptor::getFieldName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldNames[] = {
        "info",
        "numOfPackets",
        "PSORoutingPacketKind",
        "schedule",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int PSORoutingPacketDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='i' && strcmp(fieldName, "info")==0) return base+0;
    if (fieldName[0]=='n' && strcmp(fieldName, "numOfPackets")==0) return base+1;
    if (fieldName[0]=='P' && strcmp(fieldName, "PSORoutingPacketKind")==0) return base+2;
    if (fieldName[0]=='s' && strcmp(fieldName, "schedule")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *PSORoutingPacketDescriptor::getFieldTypeString(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldTypeString(object, field);
        field -= basedesc->getFieldCount(object);
    }
    static const char *fieldTypeStrings[] = {
        "nodeData",
        "int",
        "int",
        "int",
    };
    return (field>=0 && field<4) ? fieldTypeStrings[field] : NULL;
}

const char *PSORoutingPacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 2:
            if (!strcmp(propertyname,"enum")) return "PSORoutingPacket_Type";
            return NULL;
        default: return NULL;
    }
}

int PSORoutingPacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    PSORoutingPacket *pp = (PSORoutingPacket *)object; (void)pp;
    switch (field) {
        case 3: return pp->getScheduleArraySize();
        default: return 0;
    }
}

std::string PSORoutingPacketDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    PSORoutingPacket *pp = (PSORoutingPacket *)object; (void)pp;
    switch (field) {
        case 0: {std::stringstream out; out << pp->getInfo(); return out.str();}
        case 1: return long2string(pp->getNumOfPackets());
        case 2: return long2string(pp->getPSORoutingPacketKind());
        case 3: return long2string(pp->getSchedule(i));
        default: return "";
    }
}

bool PSORoutingPacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    PSORoutingPacket *pp = (PSORoutingPacket *)object; (void)pp;
    switch (field) {
        case 1: pp->setNumOfPackets(string2long(value)); return true;
        case 2: pp->setPSORoutingPacketKind(string2long(value)); return true;
        case 3: pp->setSchedule(i,string2long(value)); return true;
        default: return false;
    }
}

const char *PSORoutingPacketDescriptor::getFieldStructName(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructName(object, field);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 0: return opp_typename(typeid(nodeData));
        default: return NULL;
    };
}

void *PSORoutingPacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    PSORoutingPacket *pp = (PSORoutingPacket *)object; (void)pp;
    switch (field) {
        case 0: return (void *)(&pp->getInfo()); break;
        default: return NULL;
    }
}


