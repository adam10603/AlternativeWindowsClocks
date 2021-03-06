# Alternative Windows Clocks
![Version](https://img.shields.io/badge/Version-1.1-green.svg) ![License](https://img.shields.io/badge/License-WTFPL%20v2-blue.svg)

C++ clock classes for Windows that implement [`TrivialClock`](https://en.cppreference.com/w/cpp/named_req/TrivialClock) requirements (compatible with [`std::chrono`](https://en.cppreference.com/w/cpp/chrono)).

They can be used with my [stopwatch class](https://github.com/adam10603/CPPStopwatch) for example, but any code using [`std::chrono`](https://en.cppreference.com/w/cpp/chrono) clocks will be able to use these as well.


## precision_monotonic_clock


Property | Value
-|-
Underlying API | [`QueryPerformanceCounter`](https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter)
`is_steady` | `true`
Precision | 100ns
Resolution | <1μs (typically 100ns)

This clock wraps [`QueryPerformanceCounter`](https://docs.microsoft.com/en-us/windows/win32/api/profileapi/nf-profileapi-queryperformancecounter), and is very similar to Microsoft's current [implementation](https://github.com/microsoft/STL/blob/a9321cfe53ea31a7e197d5d8336167d6ca3de8b6/stl/inc/chrono#L683) of `std::chrono::steady_clock`, which also relies on the same API.

The differences from Microsoft's are that this version has the retrieval of both the frquency and the counter inlined (enabling better compiler optimization), and it also includes a very low-overhead special case for a frequency of 10<sup>7</sup>, which is always the case starting with Windows 10 1809, as far as I know.

This is a direct replacement for `std::chrono::steady_clock`. Note that the `period` is different between them, but this shouldn't require any change in properly written code.

It can be useful in performance-critical code that needs high-precision interval measurements while maintaining compatibility with `std::chrono`, as niche as that might be.

Performance is around 1.1x to 2.5x faster than Microsoft's `std::chrono::steady_clock` depending on use case, at least in my rudimentary benchmarks.


## fast_monotonic_clock


Property | Value
-|-
Underlying API | [`GetTickCount64`](https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-gettickcount64)
`is_steady` | `true`
Precision | 1ms
Resolution | Typically 10-16ms

This clock wraps [`GetTickCount64`](https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-gettickcount64), which isn't used in any of Microsoft's implementations of `std::chrono` clocks.

Although this clock can also replace `std::chrono::steady_clock` in existing code, there are some differences that make this more of a separate option.

It is much more performant than `precision_monotonic_clock`, however, it has a much lower precision and resolution.

This clock can be useful in performance-critical code that needs interval measurements where a precision of around 16ms is sufficient.

Performance is around 10x faster than `precision_monotonic_clock`, at least in my rudimentary benchmarks.


## Version history


* v1.0
  * Initial release.
* v1.1
  * Inlined everything in `precision_monotonic_clock::now()`.

_____________________
![WTFPL](http://www.wtfpl.net/wp-content/uploads/2012/12/wtfpl-badge-2.png) Licensed under WTFPL v2 (see [COPYING](COPYING)).