#ifndef __PROGTEST__
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#endif /* __PROGTEST__ */

// Class CString derived from BIE-PA2 Proseminar 03 at
// https://gitlab.fit.cvut.cz/BI-PA2/b242/proseminars/-/blob/main/pro03/main.cpp?ref_type=heads

class CString {
 public:
  CString(const char* str = "") {
    this->m_Length = 0;
    this->m_Capacity = 0;
    this->m_Data = nullptr;

    append(str, strlen(str));
  }

  CString(const CString& oth) {
    this->m_Length = 0;
    this->m_Capacity = 0;
    this->m_Data = nullptr;

    append(oth.m_Data, strlen(oth.m_Data));
  }

  ~CString() { delete[] m_Data; }

  CString& operator=(const CString& oth) {
    if (this != &oth) {
      delete[] m_Data;
      m_Data = new char[strlen(oth.m_Data) + 1];
      strcpy(m_Data, oth.m_Data);
    }
    return *this;
  }

  bool operator==(const CString& oth) const {
    return strcmp(m_Data, oth.m_Data) == 0;
  }

  // For ordering
  int CompareTo(const CString& oth) const { return strcmp(m_Data, oth.m_Data); }

  const char* c_str() const { return m_Data; }

 private:
  size_t m_Length;
  size_t m_Capacity;
  char* m_Data;

  void append(const char* otherData, size_t otherLength) {
    if (m_Length + otherLength + 1 > m_Capacity) {
      while (m_Length + otherLength + 1 > m_Capacity) {
        m_Capacity = m_Capacity * 1.5 + 10;
      }
      char* newData = new char[m_Capacity];
      if (m_Data != nullptr) memcpy(newData, m_Data, m_Length + 1);
      delete[] m_Data;
      if (m_Data == otherData) otherData = newData;
      m_Data = newData;
    }

    memmove(m_Data + m_Length, otherData, otherLength + 1);
    m_Length += otherLength;
  }
};

struct CDomicile {
  CString m_Date, m_Street, m_City;
};

class CCitizen {
 public:
  CCitizen(const CString& id, const CString& name, const CString& surname) {
    this->m_ID = id;
    this->m_Name = name;
    this->m_Surname = surname;
    this->m_Capacity = 4;
    this->m_Count = 0;
    m_History = new CDomicile[m_Capacity];
  }

  CCitizen(const CCitizen& oth) {
    this->m_ID = oth.m_ID;
    this->m_Name = oth.m_Name;
    this->m_Surname = oth.m_Surname;
    this->m_Capacity = oth.m_Capacity;
    this->m_Count = oth.m_Count;

    m_History = new CDomicile[m_Capacity];
    for (size_t i = 0; i < m_Count; ++i) m_History[i] = oth.m_History[i];
  }

  ~CCitizen() { delete[] m_History; }

  CCitizen& operator=(const CCitizen& other) {
    if (this != &other) {
      delete[] m_History;
      m_ID = other.m_ID;
      m_Name = other.m_Name;
      m_Surname = other.m_Surname;
      m_Capacity = other.m_Capacity;
      m_Count = other.m_Count;

      m_History = new CDomicile[m_Capacity];
      for (size_t i = 0; i < m_Count; ++i) m_History[i] = other.m_History[i];
    }
    return *this;
  }

  bool AddDomicile(const CString& date, const CString& street,
                   const CString& city) {
    if (HasDomicileOn(date)) return false;

    if (m_Count == m_Capacity) {
      CDomicile* tmp = new CDomicile[m_Capacity * 2];
      for (size_t i = 0; i < m_Count; ++i) {
        tmp[i] = m_History[i];
      }
      delete[] m_History;
      m_History = tmp;
      m_Capacity *= 2;
    }
    m_History[m_Count++] = CDomicile{date, street, city};
    return true;
  }

  bool HasDomicileOn(const CString& date) const {
    for (size_t i = 0; i < m_Count; ++i) {
      if (m_History[i].m_Date == date) {
        return true;
      }
    }
    return false;
  }

  void Print(std::ostream& os) const {
    os << m_ID.c_str() << " " << m_Name.c_str() << " " << m_Surname.c_str()
       << std::endl;

    // sort by date in ascending
    for (size_t i = 0; i < m_Count; ++i) {
      for (size_t j = i + 1; j < m_Count; ++j) {
        if (m_History[j].m_Date.CompareTo(m_History[i].m_Date) < 0) {
          CDomicile tmp = m_History[i];
          m_History[i] = m_History[j];
          m_History[j] = tmp;
        }
      }
    }

    for (size_t i = 0; i < m_Count; ++i) {
      os << m_History[i].m_Date.c_str() << " " << m_History[i].m_Street.c_str()
         << " " << m_History[i].m_City.c_str() << std::endl;
    }
  }

  const CString& ID() const { return m_ID; }

 private:
  CString m_ID, m_Name, m_Surname;
  CDomicile* m_History;
  size_t m_Capacity, m_Count;
};

class CCitizenHandle {
 public:
  CCitizenHandle() {
    this->m_Citizen = nullptr;
    this->m_ReferenceItr = nullptr;
  }

  explicit CCitizenHandle(CCitizen* citizen) {
    this->m_Citizen = citizen;
    this->m_ReferenceItr = new size_t(1);
  }

  CCitizenHandle(const CCitizenHandle& oth) {
    this->m_Citizen = oth.m_Citizen;
    this->m_ReferenceItr = oth.m_ReferenceItr;
    if (m_ReferenceItr != nullptr) {
      ++(*m_ReferenceItr);
    }
  }

  ~CCitizenHandle() {
    if (m_ReferenceItr != nullptr) {
      --(*m_ReferenceItr);
      if (*m_ReferenceItr == 0) {
        delete m_Citizen;
        delete m_ReferenceItr;
      }
    }
  }

  CCitizenHandle& operator=(const CCitizenHandle& oth) {
    if (this != &oth) {
      if (m_ReferenceItr != nullptr) {
        --(*m_ReferenceItr);
        if (*m_ReferenceItr == 0) {
          delete m_Citizen;
          delete m_ReferenceItr;
        }
      }
      m_Citizen = oth.m_Citizen;
      m_ReferenceItr = oth.m_ReferenceItr;
      if (m_ReferenceItr != nullptr) {
        ++(*m_ReferenceItr);
      }
    }
    return *this;
  }

  CCitizen* get_Var() {
    if (m_ReferenceItr != nullptr) {
      if (*m_ReferenceItr > 1) {
        --(*m_ReferenceItr);
        m_Citizen = new CCitizen(*m_Citizen);
        m_ReferenceItr = new size_t(1);
      }
    }
    return m_Citizen;
  }

  const CCitizen* get() const { return m_Citizen; }

  CCitizen* get() { return m_Citizen; }

 private:
  CCitizen* m_Citizen;
  size_t* m_ReferenceItr;
};

class CRegister {
 public:
  CRegister();
  CRegister(const CRegister& oth);
  ~CRegister();
  CRegister& operator=(const CRegister& oth);

  bool add(const char id[], const char name[], const char surname[],
           const char date[], const char street[], const char city[]);
  bool resettle(const char id[], const char date[], const char street[],
                const char city[]);
  bool print(std::ostream& os, const char id[]) const;

 private:
  size_t FindCitizenIndex(const CString& id) const;
  // CCitizen** m_Citizens;
  CCitizenHandle* m_Citizens;
  size_t m_Capacity, m_Count;
};

CRegister::CRegister() {
  this->m_Capacity = 2;
  this->m_Count = 0;
  this->m_Citizens = new CCitizenHandle[m_Capacity];
}

CRegister::CRegister(const CRegister& oth) {
  this->m_Capacity = oth.m_Capacity;
  this->m_Count = oth.m_Count;
  this->m_Citizens = new CCitizenHandle[oth.m_Capacity];

  for (size_t i = 0; i < oth.m_Count; ++i) {
    this->m_Citizens[i] = oth.m_Citizens[i];
  }
}

CRegister::~CRegister() { delete[] m_Citizens; }

CRegister& CRegister::operator=(const CRegister& oth) {
  if (this != &oth) {
    delete[] m_Citizens;

    m_Capacity = oth.m_Capacity;
    m_Count = oth.m_Count;

    m_Citizens = new CCitizenHandle[m_Capacity];
    for (size_t i = 0; i < m_Count; ++i) {
      m_Citizens[i] = oth.m_Citizens[i];
    }
  }
  return *this;
}

bool CRegister::add(const char id[], const char name[], const char surname[],
                    const char date[], const char street[], const char city[]) {
  CString cid(id);
  size_t idx = FindCitizenIndex(cid);
  if (idx < m_Count && m_Citizens[idx].get()->ID() == cid) return false;

  if (m_Count == m_Capacity) {
    CCitizenHandle* tmp = new CCitizenHandle[m_Capacity * 2];
    for (size_t i = 0; i < idx; ++i) tmp[i] = m_Citizens[i];
    for (size_t i = m_Count; i > idx; --i) tmp[i] = m_Citizens[i - 1];
    delete[] m_Citizens;
    m_Citizens = tmp;
    m_Capacity *= 2;
  } else {
    for (size_t i = m_Count; i > idx; --i) {
      m_Citizens[i] = m_Citizens[i - 1];
    }
  }

  m_Citizens[idx] = CCitizenHandle(
      new CCitizen(CString(id), CString(name), CString(surname)));
  m_Citizens[idx].get_Var()->AddDomicile(CString(date), CString(street),
                                         CString(city));
  ++m_Count;
  return true;
}

bool CRegister::resettle(const char id[], const char date[],
                         const char street[], const char city[]) {
  CString cid(id);
  size_t idx = FindCitizenIndex(cid);

  if (idx >= m_Count || !(m_Citizens[idx].get_Var()->ID() == cid)) return false;
  return m_Citizens[idx].get_Var()->AddDomicile(CString(date), CString(street),
                                                CString(city));
}

bool CRegister::print(std::ostream& os, const char id[]) const {
  CString cid(id);
  size_t idx = FindCitizenIndex(cid);

  if (idx >= m_Count || !(m_Citizens[idx].get_Var()->ID() == cid)) return false;

  m_Citizens[idx].get_Var()->Print(os);
  return true;
}

size_t CRegister::FindCitizenIndex(const CString& id) const {
  size_t left = 0;
  size_t right = m_Count;

  while (left < right) {
    size_t mid = left + (right - left) / 2;
    int cmp = m_Citizens[mid].get_Var()->ID().CompareTo(id);
    if (cmp == 0) {
      return mid;
    } else if (cmp < 0) {
      left = mid + 1;
    } else {
      right = mid;
    }
  }
  return left;
}

#ifndef __PROGTEST__
int main() {
  char lID[12], lDate[12], lName[50], lSurname[50], lStreet[50], lCity[50];
  std::ostringstream oss;
  CRegister a;
  assert(a.add("123456/7890", "John", "Smith", "2000-01-01", "Main street",
               "Seattle") == true);
  assert(a.add("987654/3210", "Freddy", "Kruger", "2001-02-03", "Elm street",
               "Sacramento") == true);
  assert(a.resettle("123456/7890", "2003-05-12", "Elm street", "Atlanta") ==
         true);
  assert(a.resettle("123456/7890", "2002-12-05", "Sunset boulevard",
                    "Los Angeles") == true);
  oss.str("");
  assert(a.print(oss, "123456/7890") == true);
  assert(!strcmp(oss.str().c_str(), R"###(123456/7890 John Smith
2000-01-01 Main street Seattle
2002-12-05 Sunset boulevard Los Angeles
2003-05-12 Elm street Atlanta
)###"));
  oss.str("");
  assert(a.print(oss, "987654/3210") == true);
  assert(!strcmp(oss.str().c_str(), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
)###"));
  CRegister b(a);
  assert(b.resettle("987654/3210", "2008-04-12", "Elm street", "Cinccinati") ==
         true);
  assert(a.resettle("987654/3210", "2007-02-11", "Elm street",
                    "Indianapolis") == true);
  oss.str("");
  assert(a.print(oss, "987654/3210") == true);
  assert(!strcmp(oss.str().c_str(), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2007-02-11 Elm street Indianapolis
)###"));
  oss.str("");
  assert(b.print(oss, "987654/3210") == true);
  assert(!strcmp(oss.str().c_str(), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2008-04-12 Elm street Cinccinati
)###"));
  a = b;
  assert(a.resettle("987654/3210", "2011-05-05", "Elm street",
                    "Salt Lake City") == true);
  oss.str("");
  assert(a.print(oss, "987654/3210") == true);
  assert(!strcmp(oss.str().c_str(), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2008-04-12 Elm street Cinccinati
2011-05-05 Elm street Salt Lake City
)###"));
  oss.str("");
  assert(b.print(oss, "987654/3210") == true);
  std::cout << oss.str().c_str() << std::endl;
  std::cout << "VAR:"
            << strcmp(oss.str().c_str(), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2008-04-12 Elm street Cinccinati
)###");
  assert(!strcmp(oss.str().c_str(), R"###(987654/3210 Freddy Kruger
2001-02-03 Elm street Sacramento
2008-04-12 Elm street Cinccinati
)###"));
  assert(b.add("987654/3210", "Joe", "Lee", "2010-03-17", "Abbey road",
               "London") == false);
  assert(a.resettle("987654/3210", "2001-02-03", "Second street",
                    "Milwaukee") == false);
  oss.str("");
  assert(a.print(oss, "666666/6666") == false);

  CRegister c;
  strncpy(lID, "123456/7890", sizeof(lID));
  strncpy(lName, "John", sizeof(lName));
  strncpy(lSurname, "Smith", sizeof(lSurname));
  strncpy(lDate, "2000-01-01", sizeof(lDate));
  strncpy(lStreet, "Main street", sizeof(lStreet));
  strncpy(lCity, "Seattle", sizeof(lCity));
  assert(c.add(lID, lName, lSurname, lDate, lStreet, lCity) == true);
  strncpy(lID, "987654/3210", sizeof(lID));
  strncpy(lName, "Freddy", sizeof(lName));
  strncpy(lSurname, "Kruger", sizeof(lSurname));
  strncpy(lDate, "2001-02-03", sizeof(lDate));
  strncpy(lStreet, "Elm street", sizeof(lStreet));
  strncpy(lCity, "Sacramento", sizeof(lCity));
  assert(c.add(lID, lName, lSurname, lDate, lStreet, lCity) == true);
  strncpy(lID, "123456/7890", sizeof(lID));
  strncpy(lDate, "2003-05-12", sizeof(lDate));
  strncpy(lStreet, "Elm street", sizeof(lStreet));
  strncpy(lCity, "Atlanta", sizeof(lCity));
  assert(c.resettle(lID, lDate, lStreet, lCity) == true);
  strncpy(lID, "123456/7890", sizeof(lID));
  strncpy(lDate, "2002-12-05", sizeof(lDate));
  strncpy(lStreet, "Sunset boulevard", sizeof(lStreet));
  strncpy(lCity, "Los Angeles", sizeof(lCity));
  assert(c.resettle(lID, lDate, lStreet, lCity) == true);
  oss.str("");
  assert(c.print(oss, "123456/7890") == true);
  assert(!strcmp(oss.str().c_str(), R"###(123456/7890 John Smith
2000-01-01 Main street Seattle
2002-12-05 Sunset boulevard Los Angeles
2003-05-12 Elm street Atlanta
)###"));

  CRegister r1;
  r1.add("888888/8888", "Bob", "Builder", "2020-01-01", "Fixit St",
         "Buildtown");

  for (int i = 0; i < 10; ++i) {
    CRegister rCopy = r1;
    std::ostringstream out;
    assert(rCopy.print(out, "888888/8888"));
  }

  std::cout << "Asserts passed." << std::endl;

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
