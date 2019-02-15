#pragma once
#include <multihash/digest.h>
#include <multihash/multihash.h>
#include <cassert>

namespace multihash {

class function {
 public:
  explicit function(algorithm::code_type algorithm_code = algorithm::sha2_256);

  /** Compute the hash of input */
  template <typename Input>
  multihash<std::string> operator()(Input& input);

  /** Write the hash of input into a buffer */
  template <typename Input>
  multihash<string_view> operator()(Input& input, string_span output);

  /** Returns the code of this hash */
  algorithm::code_type code() const;

  /** Returns the size of the multihash */
  std::size_t size();

 private:
  algorithm::code_type code_;
  digest digest_;
};

template <typename Input>
multihash<std::string> function::operator()(Input& input) {
  auto output = std::string(size(), 0);
  auto span = string_span(output);
  auto view = (*this)(input, span);
  return multihash(output);
}

template <typename Input>
multihash<string_view> function::operator()(Input& input, string_span output) {
  auto output_size = this->size();
  auto digest_size = digest_.size();
  assert(output_size <= output.size());
  using multihash = multihash<string_view>;
  auto length = multihash::write(output, code(), digest_size);
  auto span = string_span(&output[length], digest_size);
  digest_(input, span);
  auto view = string_view(&output[length], digest_size);
  return multihash::create(output, code(), view);
}

}  // namespace multihash
