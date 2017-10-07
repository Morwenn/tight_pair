`tight_pair` is a C++17 compressed pair class, trying to make its best to pack two elements together. It should
eventually provide the following features:
- [x] EBCO for empty classes
- [x] two elements of the same type are stored as an array
- [x] tuple-like access
- [ ] `get<T>`
- [ ] construction from tuple-like classes
- [ ] works with structured bindings
- [x] `std::pair`-like *EXPLICIT* constructors
- [ ] improved comparison operations when possible
- [ ] `noexcept` guarantees as strong as possible
- [x] a trivial destructor when possible
- [ ] plays nice with `std::reference_wrapper`
- [x] piecewise construction with `std::piecewise_construct`
- [ ] `constexpr` wherever possible
- [ ] smart deduction guides to replace `std::make_pair`
- [ ] includes conversions to and from `std::pair` and `std::tuple`

It only provides a reduced C++17 interface, so the following features aren't available:
- `first` and `second` members
- `first_type` and `second_type` type members
- `make_pair` factory function

It's currently a work-in-progress, don't expect it to work just yet