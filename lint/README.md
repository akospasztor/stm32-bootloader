# Linting

Cppcheck is used for linting the firmware project and perform static code
analysis. See:
[https://cppcheck.sourceforge.io/manual.pdf](https://cppcheck.sourceforge.io/manual.pdf)

The `misra.json` file contains the extra arguments required for running the
MISRA checks with Cppcheck.

The official MISRA headlines for Cppcheck can be obtained at:
[https://gitlab.com/MISRA/MISRA-C/MISRA-C-2012/tools/-/tree/main](https://gitlab.com/MISRA/MISRA-C/MISRA-C-2012/tools/-/tree/main)

## Manual execution

Make sure that the build and output directories exist because Cppcheck does not
create them automatically:

```shell
mkdir -p build/Debug/cppcheck/report
```

Run Cppcheck:

```shell
cppcheck \
--project=build/Debug/compile_commands.json \
--cppcheck-build-dir=build/Debug/cppcheck \
--checkers-report=build/Debug/cppcheck/report/checkers.txt \
--addon=lint/misra.json \
--check-level=exhaustive \
--enable=all \
--error-exitcode=1 \
--inconclusive \
--inline-suppr \
--max-ctu-depth=10 \
--safety \
--std=c11 \
--suppressions-list=lint/suppressions.txt \
-D__GNUC__ \
-j$(nproc)
```

## Inline suppression

When a suppression for a message needs to be added, it must be added with the
following format. All suppressions must have a justification.

```c
// Message:     The actual message from the static code analysis tool
// Reason:      The reason why this message pops up and why this message needs
//              to be suppressed.
// Risk:        The risk and possible unwanted side-effects by suppressing the
//              message.
// Prevention:  How can be the risk prevented/mitigated.
// cppcheck-suppress [...]
```
