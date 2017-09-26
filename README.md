`tight_pair` is a C++17 compressed pair class, trying to make its best to pack two elements together. It should
eventually provide the following features:
- [ ] EBCO for empty classes
- [ ] two elements of the same type are stored as an array
- [ ] tuple-like access
- [ ] construction from tuple-like classes
- [ ] works with structured bindings
- [ ] `std::pair`-like *EXPLICIT* constructors
- [ ] improved comparison operations when possible
- [ ] `noexcept` guarantees as strong as possible
- [ ] a trivial destructor when possible (is it even possible?)
- [ ] plays nice with `std::reference_wrapper`
- [ ] piecewise construction with `std::piecewise_construct`
- [ ] `constexpr` wherever possible
- [ ] smart deduction guides
- [ ] includes conversions to and from `std::pair` and `std::tuple`

It only provides a reduced C++17 interface, so the following features aren't available:
- `first` and `second` members
- `make_pair` factory function

It's currently a work-in-progress, 