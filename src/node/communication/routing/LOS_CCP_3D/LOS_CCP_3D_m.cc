//
// Generated file, do not edit! Created by nedtool 4.6 from src/node/communication/routing/LOS_CCP_3D/LOS_CCP_3D.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#include <iostream>
#include <sstream>
#include "LOS_CCP_3D_m.h"

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
    cEnum *e = cEnum::find("LOS_CCP_3D_Packet_Type");
    if (!e) enums.getInstance()->add(e = new cEnum("LOS_CCP_3D_Packet_Type"));
    e->insert(LOS_CCP_3D_ADV_PACKET, "LOS_CCP_3D_ADV_PACKET");
    e->insert(LOS_CCP_3D_JOIN_PACKET, "LOS_CCP_3D_JOIN_PACKET");
    e->insert(LOS_CCP_3D_TDMA_PACKET, "LOS_CCP_3D_TDMA_PACKET");
    e->insert(LOS_CCP_3D_DATA_PACKET, "LOS_CCP_3D_DATA_PACKET");
    e->insert(LOS_CCP_3D_FIND_NEIGHBOURS_PACKET, "LOS_CCP_3D_FIND_NEIGHBOURS_PACKET");
);

Register_Class(LOS_CCP_3D_Packet);

LOS_CCP_3D_Packet::LOS_CCP_3D_Packet(const char *name, int kind) : ::RoutingPacket(name,kind)
{
    this->numOfPackets_var = 0;
    this->LOS_CCP_3D_PacketKind_var = 0;
    schedule_arraysize = 0;
    this->schedule_var = 0;
}

LOS_CCP_3D_Packet::LOS_CCP_3D_Packet(const LOS_CCP_3D_Packet& other) : ::RoutingPacket(other)
{
    schedule_arraysize = 0;
    this->schedule_var = 0;
    copy(other);
}

LOS_CCP_3D_Packet::~LOS_CCP_3D_Packet()
{
    delete [] schedule_var;
}

LOS_CCP_3D_Packet& LOS_CCP_3D_Packet::operator=(const LOS_CCP_3D_Packet& other)
{
    if (this==&other) return *this;
    ::RoutingPacket::operator=(other);
    copy(other);
    return *this;
}

void LOS_CCP_3D_Packet::copy(const LOS_CCP_3D_Packet& other)
{
    this->info_var = other.info_var;
    this->numOfPackets_var = other.numOfPackets_var;
    this->LOS_CCP_3D_PacketKind_var = other.LOS_CCP_3D_PacketKind_var;
    delete [] this->schedule_var;
    this->schedule_var = (other.schedule_arraysize==0) ? NULL : new int[other.schedule_arraysize];
    schedule_arraysize = other.schedule_arraysize;
    for (unsigned int i=0; i<schedule_arraysize; i++)
        this->schedule_var[i] = other.schedule_var[i];
}

void LOS_CCP_3D_Packet::parsimPack(cCommBuffer *b)
{
    ::RoutingPacket::parsimPack(b);
    doPacking(b,this->info_var);
    doPacking(b,this->numOfPackets_var);
    doPacking(b,this->LOS_CCP_3D_PacketKind_var);
    b->pack(schedule_arraysize);
    doPacking(b,this->schedule_var,schedule_arraysize);
}

void LOS_CCP_3D_Packet::parsimUnpack(cCommBuffer *b)
{
    ::RoutingPacket::parsimUnpack(b);
    doUnpacking(b,this->info_var);
    doUnpacking(b,this->numOfPackets_var);
    doUnpacking(b,this->LOS_CCP_3D_PacketKind_var);
    delete [] this->schedule_var;
    b->unpack(schedule_arraysize);
    if (schedule_arraysize==0) {
        this->schedule_var = 0;
    } else {
        this->schedule_var = new int[schedule_arraysize];
        doUnpacking(b,this->schedule_var,schedule_arraysize);
    }
}

nodeData& LOS_CCP_3D_Packet::getInfo()
{
    return info_var;
}

void LOS_CCP_3D_Packet::setInfo(const nodeData& info)
{
    this->info_var = info;
}

int LOS_CCP_3D_Packet::getNumOfPackets() const
{
    return numOfPackets_var;
}

void LOS_CCP_3D_Packet::setNumOfPackets(int numOfPackets)
{
    this->numOfPackets_var = numOfPackets;
}

int LOS_CCP_3D_Packet::getLOS_CCP_3D_PacketKind() const
{
    return LOS_CCP_3D_PacketKind_var;
}

void LOS_CCP_3D_Packet::setLOS_CCP_3D_PacketKind(int LOS_CCP_3D_PacketKind)
{
    this->LOS_CCP_3D_PacketKind_var = LOS_CCP_3D_PacketKind;
}

void LOS_CCP_3D_Packet::setScheduleArraySize(unsigned int size)
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

unsigned int LOS_CCP_3D_Packet::getScheduleArraySize() const
{
    return schedule_arraysize;
}

int LOS_CCP_3D_Packet::getSchedule(unsigned int k) const
{
    if (k>=schedule_arraysize) throw cRuntimeError("Array of size %d indexed by %d", schedule_arraysize, k);
    return schedule_var[k];
}

void LOS_CCP_3D_Packet::setSchedule(unsigned int k, int schedule)
{
    if (k>=schedule_arraysize) throw cRuntimeError("Array of size %d indexed by %d", schedule_arraysize, k);
    this->schedule_var[k] = schedule;
}

class LOS_CCP_3D_PacketDescriptor : public cClassDescriptor
{
  public:
    LOS_CCP_3D_PacketDescriptor();
    virtual ~LOS_CCP_3D_PacketDescriptor();

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

Register_ClassDescriptor(LOS_CCP_3D_PacketDescriptor);

LOS_CCP_3D_PacketDescriptor::LOS_CCP_3D_PacketDescriptor() : cClassDescriptor("LOS_CCP_3D_Packet", "RoutingPacket")
{
}

LOS_CCP_3D_PacketDescriptor::~LOS_CCP_3D_PacketDescriptor()
{
}

bool LOS_CCP_3D_PacketDescriptor::doesSupport(cObject *obj) const
{
    return dynamic_cast<LOS_CCP_3D_Packet *>(obj)!=NULL;
}

const char *LOS_CCP_3D_PacketDescriptor::getProperty(const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : NULL;
}

int LOS_CCP_3D_PacketDescriptor::getFieldCount(void *object) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 4+basedesc->getFieldCount(object) : 4;
}

unsigned int LOS_CCP_3D_PacketDescriptor::getFieldTypeFlags(void *object, int field) const
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

const char *LOS_CCP_3D_PacketDescriptor::getFieldName(void *object, int field) const
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
        "LOS_CCP_3D_PacketKind",
        "schedule",
    };
    return (field>=0 && field<4) ? fieldNames[field] : NULL;
}

int LOS_CCP_3D_PacketDescriptor::findField(void *object, const char *fieldName) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount(object) : 0;
    if (fieldName[0]=='i' && strcmp(fieldName, "info")==0) return base+0;
    if (fieldName[0]=='n' && strcmp(fieldName, "numOfPackets")==0) return base+1;
    if (fieldName[0]=='L' && strcmp(fieldName, "LOS_CCP_3D_PacketKind")==0) return base+2;
    if (fieldName[0]=='s' && strcmp(fieldName, "schedule")==0) return base+3;
    return basedesc ? basedesc->findField(object, fieldName) : -1;
}

const char *LOS_CCP_3D_PacketDescriptor::getFieldTypeString(void *object, int field) const
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

const char *LOS_CCP_3D_PacketDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldProperty(object, field, propertyname);
        field -= basedesc->getFieldCount(object);
    }
    switch (field) {
        case 2:
            if (!strcmp(propertyname,"enum")) return "LOS_CCP_3D_Packet_Type";
            return NULL;
        default: return NULL;
    }
}

int LOS_CCP_3D_PacketDescriptor::getArraySize(void *object, int field) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getArraySize(object, field);
        field -= basedesc->getFieldCount(object);
    }
    LOS_CCP_3D_Packet *pp = (LOS_CCP_3D_Packet *)object; (void)pp;
    switch (field) {
        case 3: return pp->getScheduleArraySize();
        default: return 0;
    }
}

std::string LOS_CCP_3D_PacketDescriptor::getFieldAsString(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldAsString(object,field,i);
        field -= basedesc->getFieldCount(object);
    }
    LOS_CCP_3D_Packet *pp = (LOS_CCP_3D_Packet *)object; (void)pp;
    switch (field) {
        case 0: {std::stringstream out; out << pp->getInfo(); return out.str();}
        case 1: return long2string(pp->getNumOfPackets());
        case 2: return long2string(pp->getLOS_CCP_3D_PacketKind());
        case 3: return long2string(pp->getSchedule(i));
        default: return "";
    }
}

bool LOS_CCP_3D_PacketDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->setFieldAsString(object,field,i,value);
        field -= basedesc->getFieldCount(object);
    }
    LOS_CCP_3D_Packet *pp = (LOS_CCP_3D_Packet *)object; (void)pp;
    switch (field) {
        case 1: pp->setNumOfPackets(string2long(value)); return true;
        case 2: pp->setLOS_CCP_3D_PacketKind(string2long(value)); return true;
        case 3: pp->setSchedule(i,string2long(value)); return true;
        default: return false;
    }
}

const char *LOS_CCP_3D_PacketDescriptor::getFieldStructName(void *object, int field) const
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

void *LOS_CCP_3D_PacketDescriptor::getFieldStructPointer(void *object, int field, int i) const
{
    cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount(object))
            return basedesc->getFieldStructPointer(object, field, i);
        field -= basedesc->getFieldCount(object);
    }
    LOS_CCP_3D_Packet *pp = (LOS_CCP_3D_Packet *)object; (void)pp;
    switch (field) {
        case 0: return (void *)(&pp->getInfo()); break;
        default: return NULL;
    }
}


