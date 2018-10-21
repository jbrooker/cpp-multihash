#include <ipfs/multi/HashFunction.h>
#include <ipfs/multi/HashRawCodec.h>
#include <iomanip>
#include "gtest/gtest.h"

std::string toHexString(const ipfs::multi::Hash &hash) {
  std::ostringstream os;
  os << hash;
  return os.str();
}

TEST(Multihash, conversions) {
  /** Looking up a hash type and querying properties */
  {
    auto expected("sha1");
    auto result(ipfs::multi::HashType("sha1").name());
    EXPECT_EQ(expected, result);
  }

  {
    auto expected = static_cast<unsigned char>(ipfs::multi::HashCode::SHA1);
    auto result =
        static_cast<unsigned char>(ipfs::multi::HashType("sha1").code());
    EXPECT_EQ(expected, result);
  }

  {
    auto expected = static_cast<unsigned char>(ipfs::multi::HashCode::SHA1);
    auto hash_type = ipfs::multi::HashType(ipfs::multi::HashCode::SHA1);
    auto result = static_cast<unsigned char>(hash_type.code());
    EXPECT_EQ(expected, result);
  }

  {
    auto expected = static_cast<unsigned char>(ipfs::multi::HashCode::SHA1);
    auto hash_type = ipfs::multi::HashType("sha1");
    auto result = static_cast<unsigned char>(hash_type.code());
    EXPECT_EQ(expected, result);
  }

  {
    auto result = ipfs::multi::HashType("sha2-256").size();
    decltype(result) expected = 32;
    EXPECT_EQ(expected, result);
  }

  /** Failing to look up a hash type */
  {
    EXPECT_THROW(ipfs::multi::HashType("unknown_hash"), std::invalid_argument);
  }
  {
    EXPECT_THROW(
        ipfs::multi::HashType(static_cast<ipfs::multi::HashCode>(0x84)),
        std::invalid_argument);
  }
}

TEST(Multihash, hashing) {
  /** Decoding a hashi from raw bytes */
  std::string input("foo");
  std::istringstream input_stream(input);
  {
    ipfs::multi::HashFunction hash_function(ipfs::multi::HashCode::SHA1);
    auto hash = hash_function(input_stream);
    {
      auto expected = static_cast<unsigned char>(ipfs::multi::HashCode::SHA1);
      auto result = static_cast<unsigned char>(hash.type().code());
      EXPECT_EQ(expected, result);
    }
    {
      auto expected = "11140beec7b5ea3f0fdbc95d0dd47f3c5bc275da8a33";
      auto result = toHexString(hash);
      EXPECT_EQ(expected, result);
    }
  }
  {
    auto source_chars =
        std::string("All work and no play makes Jack a dull boy.\n");
    auto repeat_count = 1000000;
    std::stringstream os;
    for (auto i = 0; i < repeat_count; ++i) {
      os << source_chars;
    }
    auto input = os.str();

    ipfs::multi::HashFunction hash_function(ipfs::multi::HashCode::SHA1);
    auto hash = hash_function(input);
    {
      auto expected = "11147dd3e2edbe26687c037094e7cd3d8f5c5e89e9ed";
      auto result = toHexString(hash);
      EXPECT_EQ(expected, result);

      // hash again
      hash = hash_function(input);
      result = toHexString(hash);
      EXPECT_EQ(expected, result);
    }
  }
  {
    input_stream.clear();
    input_stream.seekg(0);

    auto hash_function =
        ipfs::multi::HashFunction(ipfs::multi::HashCode::SHA2_256);
    auto hash = hash_function(input_stream);
    {
      auto expected =
          static_cast<unsigned char>(ipfs::multi::HashCode::SHA2_256);
      auto result = static_cast<unsigned char>(hash.type().code());
      EXPECT_EQ(expected, result);
    }
    {
      auto expected =
          "12202c26b46b68ffc68ff99b453"
          "c1d30413413422d706483bfa0f98a5e886266e7ae";
      auto result = toHexString(hash);
      EXPECT_EQ(expected, result);

      // hash again
      hash = hash_function(input);
      result = toHexString(hash);
      EXPECT_EQ(expected, result);
    }
  }
  {
    input_stream.clear();
    input_stream.seekg(0);

    auto hash_function =
        ipfs::multi::HashFunction(ipfs::multi::HashCode::SHA2_512);
    auto hash = hash_function(input_stream);
    {
      auto expected =
          static_cast<unsigned char>(ipfs::multi::HashCode::SHA2_512);
      auto result = static_cast<unsigned char>(hash.type().code());
      EXPECT_EQ(expected, result);
    }
    {
      auto expected =
          "1340f7fbba6e0636f890e56fbbf"
          "3283e524c6fa3204ae298382d624741d0dc6638326e282c41be5"
          "e4254d8820772c5518a2c5a8c0c7f7eda19594a7eb539453e1ed7";
      auto result = toHexString(hash);
      EXPECT_EQ(expected, result);
    }
  }
  {
    auto hash_function =
        ipfs::multi::HashFunction(ipfs::multi::HashCode::SHA2_512);
    auto hash_function_b = hash_function;
    EXPECT_EQ(hash_function, hash_function_b);
  }
}

TEST(Multihash, encoding) {
  std::istringstream input_stream("foo");
  ipfs::multi::HashFunction hash_function(ipfs::multi::HashCode::SHA1);
  auto hash = hash_function(input_stream);
  ipfs::multi::HashRawCodec codec;

  auto encoded = codec(hash);
  auto decoded = codec(encoded);
  EXPECT_EQ(hash, decoded);
}

TEST(Multihash, Default) {
  ipfs::multi::HashFunction hash_function;
  EXPECT_EQ(ipfs::multi::HashCode::SHA2_256, hash_function.type().code());
}

TEST(Multihash, Inequality) {
  ipfs::multi::HashFunction hash_function;
  std::string foo("foo");
  std::string bar("bar");
  EXPECT_NE(hash_function(ipfs::multi::string_view(foo)),
            hash_function(ipfs::multi::string_view(bar)));
}

TEST(Multihash, HashConstruction) {
  ipfs::multi::HashFunction hash_function;
  std::string foo("foo");
  auto hash = hash_function(foo);
  auto expected = std::string(
      "12202c26b46b68ffc68ff99b453c1d3041341"
      "3422d706483bfa0f98a5e886266e7ae");
  EXPECT_EQ(expected, toHexString(hash));
  {  // copy assignment
    auto hash_copied = hash;
    EXPECT_EQ(expected, toHexString(hash_copied));
    EXPECT_EQ(expected, toHexString(hash));
    EXPECT_EQ(hash_copied, hash);
  }
  {  // copy construction
    ipfs::multi::Hash hash_copied(hash);
    EXPECT_EQ(expected, toHexString(hash));
    EXPECT_EQ(hash_copied, hash);
  }

  EXPECT_EQ(hash_function(foo), hash_function(foo));
  EXPECT_EQ(hash, hash_function(foo));
  EXPECT_EQ(expected, toHexString(hash_function(foo)));
}