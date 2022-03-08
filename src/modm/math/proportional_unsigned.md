# Proportional Unsigned

Unsigned integer with arbitrary digits D and proportional scaling between
instances with different digits. F.e. a ProportionalUnsigned<2> with the value
0b11 converted to a to a ProportionalUnsigned<4> converts to value 0b1111.

It's the BaseClass to all Color types (color::Gray, color::Rgb, color::Hsv, a.s.o. ) bu
may have other applications for DSP applications.

Use this handler to capture "value out of range" issues in debug mode

```cpp
static modm::Abandonment
bad_proportional_unsigned_value(const modm::AssertionInfo &info)
{
    if(!strncmp(info.name, (char*)"propun", 6)) {
        MODM_LOG_ERROR << info.name << " " << info.description << " " << info.context << modm::endl;
        return modm::Abandonment::Ignore;
    }
    return modm::Abandonment::DontCare;
}
MODM_ASSERTION_HANDLER_DEBUG(bad_proportional_unsigned_value);
```