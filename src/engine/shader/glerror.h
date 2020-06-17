#ifndef GLERROR_H
#define GLERROR_H
namespace engine {
const char *glErrorString(int err);
void printProgramLog(int programID);
void printShaderLog(int shaderID);
} // namespace engine
#endif // GLERROR_H
