# libsigrokdecode PR Porting TODOs for OpenTraceDecode

## High Priority PRs (Immediate Value)

### PR #125: SPI Flash Enhancements (karlp)
**Status**: 11 commits, comprehensive SPI flash improvements
**OpenTraceDecode Changes Needed**:
- Adapt Python decoder structure to OpenTraceDecode's decoder loading system
- Update annotation system to work with OpenTraceDecode's output format
- Modify chip database structure for OpenTraceDecode's data handling
- Adapt n-byte addressing logic for OpenTraceDecode's address handling

**Code Quality Assessment**: ⭐⭐⭐⭐⭐
- Well-structured commits with clear purposes
- Comprehensive testing mentioned
- Good error handling and validation
- Proper signed-off commits

**Decoder Dependencies**: 
- **LOW RISK**: SPI flash decoder is typically standalone
- May affect higher-level decoders that depend on SPI flash data
- No breaking changes to core SPI protocol

**Porting Tasks**:
1. Extract SPI flash decoder from libsigrokdecode
2. Adapt chip database format to OpenTraceDecode
3. Update annotation output for OpenTraceDecode format
4. Test with OpenTraceDecode's SPI decoder
5. Validate n-byte addressing with test data
6. Update documentation for new features

---

### PR #121: UART Packet Data Publishing (karlp)
**Status**: 1 commit, extends UART decoder functionality
**OpenTraceDecode Changes Needed**:
- Adapt packet output mechanism to OpenTraceDecode's data flow
- Ensure compatibility with OpenTraceDecode's Python data structures
- Update UART decoder to support both annotation and packet outputs

**Code Quality Assessment**: ⭐⭐⭐⭐
- Simple, focused change
- Clear purpose and implementation
- Minimal risk of introducing bugs

**Decoder Dependencies**:
- **MEDIUM RISK**: UART is a foundational decoder
- Many higher-level protocol decoders depend on UART
- Change is additive, shouldn't break existing functionality

**Porting Tasks**:
1. Review OpenTraceDecode's UART decoder structure
2. Add packet data output alongside existing annotations
3. Test with dependent decoders (RS232, Modbus, etc.)
4. Validate packet framing accuracy
5. Update UART decoder documentation

---

### PR #123: Test Framework Improvements (neuschaefer)
**Status**: Replace deprecated test calls
**OpenTraceDecode Changes Needed**:
- Update test framework to use modern testing patterns
- Ensure compatibility with OpenTraceDecode's test infrastructure

**Code Quality Assessment**: ⭐⭐⭐⭐
- Maintenance improvement
- Follows modern testing practices
- Low risk change

**Decoder Dependencies**:
- **NO RISK**: Test infrastructure change only
- Improves overall project maintainability

**Porting Tasks**:
1. Review OpenTraceDecode's current test framework
2. Apply similar deprecated call replacements
3. Ensure all tests pass with updates
4. Document testing best practices

---

## Medium Priority PRs (New Protocol Support)

### PR #120: OpenTherm Protocol Decoder (otdecoderman)
**Status**: 1 commit, new protocol decoder
**OpenTraceDecode Changes Needed**:
- Adapt decoder structure to OpenTraceDecode's decoder API
- Update metadata and registration for OpenTraceDecode
- Ensure timing and glitch filtering work with OpenTraceDecode's sample handling

**Code Quality Assessment**: ⭐⭐⭐
- New protocol implementation
- Includes glitch filtering and timing validation
- Author notes missing test data

**Decoder Dependencies**:
- **NO RISK**: New standalone decoder
- No existing dependencies

**Porting Tasks**:
1. Extract OpenTherm decoder implementation
2. Adapt to OpenTraceDecode's decoder API
3. Create test data for validation
4. Add to OpenTraceDecode's decoder registry
5. Document OpenTherm protocol support

---

### PR #116: KeeLoq Decoder (rzondr)
**Status**: New security protocol decoder
**OpenTraceDecode Changes Needed**:
- Port decoder to OpenTraceDecode's Python environment
- Adapt cryptographic operations if any
- Update for OpenTraceDecode's annotation system

**Code Quality Assessment**: ⭐⭐⭐
- Specialized security protocol
- Requires domain expertise for validation

**Decoder Dependencies**:
- **NO RISK**: New standalone decoder
- May be useful for automotive/security analysis

**Porting Tasks**:
1. Review KeeLoq protocol implementation
2. Port to OpenTraceDecode decoder format
3. Validate cryptographic accuracy
4. Add comprehensive test cases
5. Document security protocol features

---

### PR #117: AD5593R Decoder (Adrian-Stanea)
**Status**: New I2C device decoder
**OpenTraceDecode Changes Needed**:
- Ensure I2C dependency compatibility
- Adapt register decoding for OpenTraceDecode
- Update device-specific annotations

**Code Quality Assessment**: ⭐⭐⭐
- Device-specific decoder
- Requires hardware knowledge for validation

**Decoder Dependencies**:
- **LOW RISK**: Depends on I2C decoder
- I2C is stable in OpenTraceDecode

**Porting Tasks**:
1. Port AD5593R decoder implementation
2. Validate I2C dependency compatibility
3. Test register decoding accuracy
4. Add device documentation
5. Create test vectors

---

## Lower Priority PRs (Build System & Infrastructure)

### PR #110: CMake Support (Murmele)
**Status**: 5 commits, alternative build system
**OpenTraceDecode Changes Needed**:
- **NOT APPLICABLE**: OpenTraceDecode uses Meson
- Could be reference for build system improvements

**Code Quality Assessment**: ⭐⭐⭐⭐
- Well-structured CMake implementation
- Good IDE integration
- Modern CMake practices

**Decoder Dependencies**:
- **NO RISK**: Build system only

**Porting Tasks**:
1. **SKIP**: OpenTraceDecode uses Meson
2. Review for potential Meson improvements
3. Consider IDE integration lessons

---

### PR #122: SPI Decoder Cleanup (agarmash)
**Status**: SPI decoder refactoring
**OpenTraceDecode Changes Needed**:
- Review cleanup changes for applicability
- Apply similar improvements to OpenTraceDecode's SPI decoder

**Code Quality Assessment**: ⭐⭐⭐⭐
- Code quality improvement
- Refactoring for maintainability

**Decoder Dependencies**:
- **MEDIUM RISK**: SPI is foundational
- Many decoders depend on SPI

**Porting Tasks**:
1. Review SPI cleanup changes
2. Apply applicable improvements to OpenTraceDecode
3. Test with all SPI-dependent decoders
4. Validate performance improvements

---

## Implementation Priority Order

1. **PR #123**: Test improvements (low risk, high value)
2. **PR #121**: UART packet data (medium risk, high value)
3. **PR #125**: SPI flash enhancements (medium risk, very high value)
4. **PR #120**: OpenTherm decoder (low risk, medium value)
5. **PR #122**: SPI cleanup (medium risk, medium value)
6. **PR #116**: KeeLoq decoder (low risk, low value)
7. **PR #117**: AD5593R decoder (low risk, low value)

## General Porting Considerations

### OpenTraceDecode Compatibility Requirements
- Python 3.8+ compatibility (vs libsigrokdecode's 3.2-3.6)
- Meson build system integration
- OpenTraceDecode's annotation format
- Modern Python practices and type hints
- OpenTraceDecode's decoder registration system

### Testing Strategy
- Create test data for each ported decoder
- Validate against original libsigrokdecode output
- Test with OpenTraceDecode's existing decoder ecosystem
- Performance benchmarking for complex decoders

### Documentation Updates
- Update decoder documentation for OpenTraceDecode
- Add protocol specifications where missing
- Create usage examples
- Update API documentation
