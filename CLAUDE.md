# Embedded Cross-Compile Agent - Project Context

## Project Overview

This is an AI-assisted embedded Linux cross-compilation agent project.
It automates diagnosis and fixing of common pain points in ARM/RISC-V/MIPS cross-compilation workflows.

## Key Documents

- `README.md` - Project overview and quick start
- `pain-points.md` - Detailed pain point analysis (reference this when encountering issues)
- `diagnose.sh` - Run this first to detect environment issues
- `fix.sh` - Run this to auto-fix detected issues
- `PROMPT.md` - New conversation startup prompts

## Common Workflow

When helping with embedded cross-compilation projects:

1. **First**: Run `./diagnose.sh` to check the environment
2. **If issues found**: Run `./fix.sh` to auto-fix
3. **If build fails**: Check `pain-points.md` for the specific error
4. **If still stuck**: Manually investigate and fix

## Pain Points to Always Check

When a user reports a cross-compilation issue, check these in order:

1. **CRLF line endings** - `file configure | grep CRLF`
2. **Recursive directories** - `find . -path "*/alsa/alsa" -type d`
3. **Path too long** - `find . -type f | awk '{print length, $0}' | sort -rn | head -1`
4. **pkg-config hardcoded paths** - `grep prefix= compiled/*/build/lib/pkgconfig/*.pc`
5. **Wrong architecture** - `file binary | grep ARM`
6. **Missing environment variables** - `echo $CC $CXX $AR`
7. **Missing runtime libraries** - `LD_LIBRARY_PATH=lib ldd bin/app | grep "not found"`

## Auto-Fix Patterns

### Pattern: CRLF Error
```
Symptom: /bin/sh^M: interpreter error
Fix: dos2unix <file>
```

### Pattern: Recursive Directory
```
Symptom: alsa/alsa/alsa/... (40+ levels)
Fix: find . -path "*/alsa/alsa" -type d -exec rm -rf {} +
```

### Pattern: pkg-config Not Found
```
Symptom: configure: error: Sufficiently new version of libXXX not found
Fix: export PKG_CONFIG_PATH=$(pwd)/compiled/libXXX/build/lib/pkgconfig
```

### Pattern: Wrong Architecture
```
Symptom: file shows x86-64 instead of ARM
Fix: export CC=arm-linux-gnueabi-gcc (and other vars)
```

### Pattern: Qt OpenGL Error
```
Symptom: 'QOpenGLTexture' does not name a type
Fix: ./configure -no-opengl -no-vulkan
```

## SSH Access

To connect to a development VM:
```bash
ssh user@ip_address
```

For non-interactive SSH (when password is required):
```bash
# Create password helper
echo '#!/bin/bash' > /tmp/sshpass.sh
echo 'echo "password"' >> /tmp/sshpass.sh
chmod +x /tmp/sshpass.sh

# Use SSH_ASKPASS
SSH_ASKPASS=/tmp/sshpass.sh DISPLAY=:0 ssh user@host "command"
```

## Target Platforms

Supported target architectures:
- ARM 32-bit: `arm-linux-gnueabi` / `arm-linux-gnueabihf`
- ARM 64-bit: `aarch64-linux-gnu`
- RISC-V: `riscv64-linux-gnu`
- MIPS: `mips-linux-gnu`

## Deployment Checklist

Before deploying to target device:
- [ ] All binaries are correct architecture (`file` check)
- [ ] All runtime libraries included (`ldd` check)
- [ ] No hardcoded paths in scripts
- [ ] LD_LIBRARY_PATH configured
- [ ] QT_QPA_PLATFORM set (for Qt apps)
- [ ] Startup script created and executable
