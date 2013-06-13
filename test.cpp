
#include <cereal/cereal.hpp>
#include <cereal/binary_archive/binary_archive.hpp>
#include <cereal/binary_archive/string.hpp>
#include <cereal/json_archive/json_archive.hpp>
#include <cereal/binary_archive/shared_ptr.hpp>

#include <cxxabi.h>
#include <sstream>
#include <fstream>
#include <cassert>

// ###################################
struct Test1
{
  int a;

  template<class Archive>
  void serialize(Archive & ar)
  {
    ar & CEREAL_NVP(a);
  }
};

// ###################################
struct Test2
{
  int a;

  template<class Archive>
  void save(Archive & ar) const
  {
    ar & CEREAL_NVP(a);
  }

  template<class Archive>
  void load(Archive & ar)
  {
    ar & CEREAL_NVP(a);
  }
};

// ###################################
struct Test3
{
  int a;
};

template<class Archive>
void serialize(Archive & ar, Test3 & t)
{
  ar & CEREAL_NVP(t.a);
}

namespace test4
{
  // ###################################
  struct Test4
  {
    int a;
  };

  template<class Archive>
  void save(Archive & ar, Test4 const & t)
  {
    ar & CEREAL_NVP(t.a);
  }

  template<class Archive>
  void load(Archive & ar, Test4 & t)
  {
    ar & CEREAL_NVP(t.a);
  }
}

struct Everything
{
  int x;
  int y;
  Test1 t1;
  Test2 t2;
  Test3 t3;
  test4::Test4 t4;
  std::string s;

  template<class Archive>
  void serialize(Archive & ar)
  {
    ar & CEREAL_NVP(x);
    ar & CEREAL_NVP(y);
    ar & CEREAL_NVP(t1);
    ar & CEREAL_NVP(t2);
    ar & CEREAL_NVP(t3);
    ar & CEREAL_NVP(t4);
    ar & CEREAL_NVP(s);
  }

  bool operator==(Everything const & o)
  {
    return
      x == o.x &&
      y == o.y &&
      t1.a == o.t1.a &&
      t2.a == o.t2.a &&
      t3.a == o.t3.a &&
      t4.a == o.t4.a &&
      s == o.s;
  }
};

// ######################################################################
int main()
{
  Everything e_out;
  e_out.x = 99;
  e_out.y = 100;
  e_out.t1 = {1};
  e_out.t2 = {2};
  e_out.t3 = {3};
  e_out.t4 = {4};
  e_out.s = "Hello, World!";

  {
    std::ofstream os("out.txt");
    cereal::BinaryOutputArchive archive(os);
    archive & CEREAL_NVP(e_out);
  }

  Everything e_in;

  {
    std::ifstream is("out.txt");
    cereal::BinaryInputArchive archive(is);
    archive & CEREAL_NVP(e_in);
  }

  assert(e_in == e_out);

  //cereal::JSONOutputArchive json(std::cout);

  //std::string hello = "Hello, World!";
  //json & CEREAL_NVP(hello);
  //json & CEREAL_NVP(e_out); <<< Need to figure out how to recurse!

  {
    std::ofstream os("ptr.txt");
    cereal::BinaryOutputArchive archive(os);
    std::shared_ptr<int> xptr1 = std::make_shared<int>(5);
    std::shared_ptr<int> xptr2 = xptr1;
    std::shared_ptr<int> yptr1 = std::make_shared<int>(6);
    std::shared_ptr<int> yptr2 = yptr1;
    archive & xptr1;
    archive & xptr2;
    archive & yptr1;
    archive & yptr2;

  }
  {
    std::ifstream is("ptr.txt");
    cereal::BinaryInputArchive archive(is);
    std::shared_ptr<int> xptr1;
    std::shared_ptr<int> xptr2;
    std::shared_ptr<int> yptr1;
    std::shared_ptr<int> yptr2;
    archive & xptr1;
    archive & xptr2;
    archive & yptr1;
    archive & yptr2;

    assert(xptr1.get() == xptr2.get());
    assert(yptr1.get() == yptr2.get());
    std::cout << *xptr1 << " " << *xptr2 << std::endl;
    std::cout << *yptr1 << " " << *yptr2 << std::endl;
  }



  return 0;
}