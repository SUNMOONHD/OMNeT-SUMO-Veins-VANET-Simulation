//
// Generated file, do not edit! Created by nedtool 5.7 from veins/modules/application/SVMD/SVMDformerMessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include "SVMDformerMessage_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

namespace {
template <class T> inline
typename std::enable_if<std::is_polymorphic<T>::value && std::is_base_of<omnetpp::cObject,T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)(static_cast<const omnetpp::cObject *>(t));
}

template <class T> inline
typename std::enable_if<std::is_polymorphic<T>::value && !std::is_base_of<omnetpp::cObject,T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)dynamic_cast<const void *>(t);
}

template <class T> inline
typename std::enable_if<!std::is_polymorphic<T>::value, void *>::type
toVoidPtr(T* t)
{
    return (void *)static_cast<const void *>(t);
}

}

namespace veins {

// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule to generate operator<< for shared_ptr<T>
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const std::shared_ptr<T>& t) { return out << t.get(); }

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
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

Register_Class(SVMDformerMessage)

SVMDformerMessage::SVMDformerMessage(const char *name, short kind) : ::veins::BaseFrame1609_4(name, kind)
{
}

SVMDformerMessage::SVMDformerMessage(const SVMDformerMessage& other) : ::veins::BaseFrame1609_4(other)
{
    copy(other);
}

SVMDformerMessage::~SVMDformerMessage()
{
}

SVMDformerMessage& SVMDformerMessage::operator=(const SVMDformerMessage& other)
{
    if (this == &other) return *this;
    ::veins::BaseFrame1609_4::operator=(other);
    copy(other);
    return *this;
}

void SVMDformerMessage::copy(const SVMDformerMessage& other)
{
    this->senderAddress = other.senderAddress;
    this->deltaT = other.deltaT;
    this->pseudoId = other.pseudoId;
    this->pos = other.pos;
    this->speed = other.speed;
    this->accel = other.accel;
    this->heading = other.heading;
    this->serial = other.serial;
    this->isMaliciousLabel_ = other.isMaliciousLabel_;
}

void SVMDformerMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::veins::BaseFrame1609_4::parsimPack(b);
    doParsimPacking(b,this->senderAddress);
    doParsimPacking(b,this->deltaT);
    doParsimPacking(b,this->pseudoId);
    doParsimPacking(b,this->pos);
    doParsimPacking(b,this->speed);
    doParsimPacking(b,this->accel);
    doParsimPacking(b,this->heading);
    doParsimPacking(b,this->serial);
    doParsimPacking(b,this->isMaliciousLabel_);
}

void SVMDformerMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::veins::BaseFrame1609_4::parsimUnpack(b);
    doParsimUnpacking(b,this->senderAddress);
    doParsimUnpacking(b,this->deltaT);
    doParsimUnpacking(b,this->pseudoId);
    doParsimUnpacking(b,this->pos);
    doParsimUnpacking(b,this->speed);
    doParsimUnpacking(b,this->accel);
    doParsimUnpacking(b,this->heading);
    doParsimUnpacking(b,this->serial);
    doParsimUnpacking(b,this->isMaliciousLabel_);
}

const LAddress::L2Type& SVMDformerMessage::getSenderAddress() const
{
    return this->senderAddress;
}

void SVMDformerMessage::setSenderAddress(const LAddress::L2Type& senderAddress)
{
    this->senderAddress = senderAddress;
}

omnetpp::simtime_t SVMDformerMessage::getDeltaT() const
{
    return this->deltaT;
}

void SVMDformerMessage::setDeltaT(omnetpp::simtime_t deltaT)
{
    this->deltaT = deltaT;
}

long SVMDformerMessage::getPseudoId() const
{
    return this->pseudoId;
}

void SVMDformerMessage::setPseudoId(long pseudoId)
{
    this->pseudoId = pseudoId;
}

const Coord& SVMDformerMessage::getPos() const
{
    return this->pos;
}

void SVMDformerMessage::setPos(const Coord& pos)
{
    this->pos = pos;
}

const Coord& SVMDformerMessage::getSpeed() const
{
    return this->speed;
}

void SVMDformerMessage::setSpeed(const Coord& speed)
{
    this->speed = speed;
}

const Coord& SVMDformerMessage::getAccel() const
{
    return this->accel;
}

void SVMDformerMessage::setAccel(const Coord& accel)
{
    this->accel = accel;
}

const Coord& SVMDformerMessage::getHeading() const
{
    return this->heading;
}

void SVMDformerMessage::setHeading(const Coord& heading)
{
    this->heading = heading;
}

long SVMDformerMessage::getSerial() const
{
    return this->serial;
}

void SVMDformerMessage::setSerial(long serial)
{
    this->serial = serial;
}

bool SVMDformerMessage::isMaliciousLabel() const
{
    return this->isMaliciousLabel_;
}

void SVMDformerMessage::setIsMaliciousLabel(bool isMaliciousLabel)
{
    this->isMaliciousLabel_ = isMaliciousLabel;
}

class SVMDformerMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
    enum FieldConstants {
        FIELD_senderAddress,
        FIELD_deltaT,
        FIELD_pseudoId,
        FIELD_pos,
        FIELD_speed,
        FIELD_accel,
        FIELD_heading,
        FIELD_serial,
        FIELD_isMaliciousLabel,
    };
  public:
    SVMDformerMessageDescriptor();
    virtual ~SVMDformerMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(SVMDformerMessageDescriptor)

SVMDformerMessageDescriptor::SVMDformerMessageDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(veins::SVMDformerMessage)), "veins::BaseFrame1609_4")
{
    propertynames = nullptr;
}

SVMDformerMessageDescriptor::~SVMDformerMessageDescriptor()
{
    delete[] propertynames;
}

bool SVMDformerMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<SVMDformerMessage *>(obj)!=nullptr;
}

const char **SVMDformerMessageDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *SVMDformerMessageDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int SVMDformerMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 9+basedesc->getFieldCount() : 9;
}

unsigned int SVMDformerMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        0,    // FIELD_senderAddress
        FD_ISEDITABLE,    // FIELD_deltaT
        FD_ISEDITABLE,    // FIELD_pseudoId
        0,    // FIELD_pos
        0,    // FIELD_speed
        0,    // FIELD_accel
        0,    // FIELD_heading
        FD_ISEDITABLE,    // FIELD_serial
        FD_ISEDITABLE,    // FIELD_isMaliciousLabel
    };
    return (field >= 0 && field < 9) ? fieldTypeFlags[field] : 0;
}

const char *SVMDformerMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "senderAddress",
        "deltaT",
        "pseudoId",
        "pos",
        "speed",
        "accel",
        "heading",
        "serial",
        "isMaliciousLabel",
    };
    return (field >= 0 && field < 9) ? fieldNames[field] : nullptr;
}

int SVMDformerMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0] == 's' && strcmp(fieldName, "senderAddress") == 0) return base+0;
    if (fieldName[0] == 'd' && strcmp(fieldName, "deltaT") == 0) return base+1;
    if (fieldName[0] == 'p' && strcmp(fieldName, "pseudoId") == 0) return base+2;
    if (fieldName[0] == 'p' && strcmp(fieldName, "pos") == 0) return base+3;
    if (fieldName[0] == 's' && strcmp(fieldName, "speed") == 0) return base+4;
    if (fieldName[0] == 'a' && strcmp(fieldName, "accel") == 0) return base+5;
    if (fieldName[0] == 'h' && strcmp(fieldName, "heading") == 0) return base+6;
    if (fieldName[0] == 's' && strcmp(fieldName, "serial") == 0) return base+7;
    if (fieldName[0] == 'i' && strcmp(fieldName, "isMaliciousLabel") == 0) return base+8;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *SVMDformerMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "veins::LAddress::L2Type",    // FIELD_senderAddress
        "omnetpp::simtime_t",    // FIELD_deltaT
        "long",    // FIELD_pseudoId
        "veins::Coord",    // FIELD_pos
        "veins::Coord",    // FIELD_speed
        "veins::Coord",    // FIELD_accel
        "veins::Coord",    // FIELD_heading
        "long",    // FIELD_serial
        "bool",    // FIELD_isMaliciousLabel
    };
    return (field >= 0 && field < 9) ? fieldTypeStrings[field] : nullptr;
}

const char **SVMDformerMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *SVMDformerMessageDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int SVMDformerMessageDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    SVMDformerMessage *pp = (SVMDformerMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *SVMDformerMessageDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    SVMDformerMessage *pp = (SVMDformerMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string SVMDformerMessageDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    SVMDformerMessage *pp = (SVMDformerMessage *)object; (void)pp;
    switch (field) {
        case FIELD_senderAddress: {std::stringstream out; out << pp->getSenderAddress(); return out.str();}
        case FIELD_deltaT: return simtime2string(pp->getDeltaT());
        case FIELD_pseudoId: return long2string(pp->getPseudoId());
        case FIELD_pos: {std::stringstream out; out << pp->getPos(); return out.str();}
        case FIELD_speed: {std::stringstream out; out << pp->getSpeed(); return out.str();}
        case FIELD_accel: {std::stringstream out; out << pp->getAccel(); return out.str();}
        case FIELD_heading: {std::stringstream out; out << pp->getHeading(); return out.str();}
        case FIELD_serial: return long2string(pp->getSerial());
        case FIELD_isMaliciousLabel: return bool2string(pp->isMaliciousLabel());
        default: return "";
    }
}

bool SVMDformerMessageDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    SVMDformerMessage *pp = (SVMDformerMessage *)object; (void)pp;
    switch (field) {
        case FIELD_deltaT: pp->setDeltaT(string2simtime(value)); return true;
        case FIELD_pseudoId: pp->setPseudoId(string2long(value)); return true;
        case FIELD_serial: pp->setSerial(string2long(value)); return true;
        case FIELD_isMaliciousLabel: pp->setIsMaliciousLabel(string2bool(value)); return true;
        default: return false;
    }
}

const char *SVMDformerMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

void *SVMDformerMessageDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    SVMDformerMessage *pp = (SVMDformerMessage *)object; (void)pp;
    switch (field) {
        case FIELD_senderAddress: return toVoidPtr(&pp->getSenderAddress()); break;
        case FIELD_pos: return toVoidPtr(&pp->getPos()); break;
        case FIELD_speed: return toVoidPtr(&pp->getSpeed()); break;
        case FIELD_accel: return toVoidPtr(&pp->getAccel()); break;
        case FIELD_heading: return toVoidPtr(&pp->getHeading()); break;
        default: return nullptr;
    }
}

} // namespace veins

