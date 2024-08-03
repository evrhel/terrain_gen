#include "shader.h"

#include <cstdio>
#include <cstdarg>

#include "material.h"
#include "util.h"
#include "gbuffer.h"

void Shader::use() const
{
    glUseProgram(_program);
}

void Shader::load(const char *name, const char *vertexSource, const char *fragmentSource)
{
    GLuint vert, frag;
    int success;
    char infoLog[512];
    bool hasError = false;

    printf("Shader::load: Loading %s\n", name);

    /* Compile vertex shader */

    vert = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vert, 1, &vertexSource, NULL);
    glCompileShader(vert);

    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vert, 512, NULL, infoLog);
        printf("Vertex shader compilation failed: %s\n", infoLog);
        hasError = true;
        glDeleteShader(vert);
    }

    /* Compile fragment shader */

    frag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(frag, 1, &fragmentSource, NULL);
    glCompileShader(frag);

    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag, 512, NULL, infoLog);
        printf("Fragment shader compilation failed: %s\n", infoLog);
        hasError = true;
        glDeleteShader(frag);
    }

    if (hasError)
        exit(1);

    /* Link shaders */

    _program = glCreateProgram();

    glAttachShader(_program, vert);
    glAttachShader(_program, frag);
    glLinkProgram(_program);

    glGetProgramiv(_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(_program, 512, NULL, infoLog);
        printf("Shader program linking failed: %s\n", infoLog);
        exit(1);
    }

    glDeleteShader(vert);
    glDeleteShader(frag);

    _name = name;

    /* Bind standard buffers */
    bindUniformBlock("Camera", CAMERA_UNIFORM_BINDING);
    bindUniformBlock("Atmosphere", ATMOSPHERE_UNIFORM_BINDING);
}

void Shader::loadGeom(const char *name, const char *vertexSource, const char *geomSource, const char *fragmentSource)
{
    GLuint vert, geom, frag;
    int success;
    char infoLog[512];
    bool hasError = false;

    printf("Shader::loadGeom: Loading %s\n", name);

    /* Compile vertex shader */

    vert = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vert, 1, &vertexSource, NULL);
    glCompileShader(vert);

    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if (!success)
    {
		glGetShaderInfoLog(vert, 512, NULL, infoLog);
		printf("Vertex shader compilation failed: %s\n", infoLog);
		hasError = true;
		glDeleteShader(vert);
	}

    /* Compile geometry shader */

    geom = glCreateShader(GL_GEOMETRY_SHADER);

    glShaderSource(geom, 1, &geomSource, NULL);
    glCompileShader(geom);

    glGetShaderiv(geom, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(geom, 512, NULL, infoLog);
        printf("Geometry shader compilation failed: %s\n", infoLog);
        hasError = true;
        glDeleteShader(geom);
    }

    /* Compile fragment shader */

    frag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(frag, 1, &fragmentSource, NULL);
    glCompileShader(frag);

    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success)
    {
		glGetShaderInfoLog(frag, 512, NULL, infoLog);
		printf("Fragment shader compilation failed: %s\n", infoLog);
		hasError = true;
		glDeleteShader(frag);
	}

    /* Link shaders */

    _program = glCreateProgram();

    glAttachShader(_program, vert);
    glAttachShader(_program, geom);
    glAttachShader(_program, frag);

    glLinkProgram(_program);

    glGetProgramiv(_program, GL_LINK_STATUS, &success);
    if (!success)
    {
		glGetProgramInfoLog(_program, 512, NULL, infoLog);
        hasError = true;
		printf("Shader program linking failed: %s\n", infoLog);
	}

    if (hasError)
		exit(1);

    glDeleteShader(geom);
	glDeleteShader(frag);
	glDeleteShader(vert);

	_name = name;

	/* Bind standard buffers */
	bindUniformBlock("Camera", CAMERA_UNIFORM_BINDING);
	bindUniformBlock("Atmosphere", ATMOSPHERE_UNIFORM_BINDING);
}

void Shader::loadTess(const char *name, const char *vertexSource, const char *fragmentSource, const char *tessControlSource, const char *tessEvalSource)
{
    GLuint vert, frag, tcs, tes;
    int success;
    char infoLog[512];
    bool hasError = false;

    printf("Shader::loadTess: Loading %s\n", name);

    /* Compile vertex shader */

    vert = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vert, 1, &vertexSource, NULL);
    glCompileShader(vert);

    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vert, 512, NULL, infoLog);
        printf("Vertex shader compilation failed: %s\n", infoLog);
        hasError = true;
        glDeleteShader(vert);
    }

    /* Compile fragment shader */

    frag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(frag, 1, &fragmentSource, NULL);
    glCompileShader(frag);

    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag, 512, NULL, infoLog);
        printf("Fragment shader compilation failed: %s\n", infoLog);
        hasError = true;
        glDeleteShader(frag);
    }

    /* Compile tessellation control shader */

    tcs = glCreateShader(GL_TESS_CONTROL_SHADER);

    glShaderSource(tcs, 1, &tessControlSource, NULL);
    glCompileShader(tcs);

    glGetShaderiv(tcs, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(tcs, 512, NULL, infoLog);
        printf("Tessellation control shader compilation failed: %s\n", infoLog);
        hasError = true;
        glDeleteShader(tcs);
    }

    /* Compile tessellation evaluation shader */

    tes = glCreateShader(GL_TESS_EVALUATION_SHADER);

    glShaderSource(tes, 1, &tessEvalSource, NULL);
    glCompileShader(tes);

    glGetShaderiv(tes, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(tes, 512, NULL, infoLog);
        printf("Tessellation evaluation shader compilation failed: %s\n", infoLog);
        hasError = true;
        glDeleteShader(tes);
    }

    if (hasError)
        exit(1);

    /* Link shaders */

    _program = glCreateProgram();

    glAttachShader(_program, vert);
    glAttachShader(_program, frag);
    glAttachShader(_program, tcs);
    glAttachShader(_program, tes);

    glLinkProgram(_program);

    glGetProgramiv(_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(_program, 512, NULL, infoLog);
        printf("Shader program linking failed: %s\n", infoLog);
        exit(1);
    }

    glDeleteShader(tes);
    glDeleteShader(tcs);
    glDeleteShader(frag);
    glDeleteShader(vert);

    _name = name;

    /* Bind standard buffers */
    bindUniformBlock("Camera", CAMERA_UNIFORM_BINDING);
    bindUniformBlock("Atmosphere", ATMOSPHERE_UNIFORM_BINDING);
}

void Shader::setBool(const char *name, bool value)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
        glUniform1i(loc, value);
}

void Shader::setFloat(const char *name, float value)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
        glUniform1f(loc, value);
}

void Shader::setInt(const char *name, int value)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
        glUniform1i(loc, value);
}

void Shader::setVector2(const char *name, const Vector2 &value)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
        glUniform2fv(loc, 1, (float *)&value);
}

void Shader::setVector3(const char *name, const Vector3 &value)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
        glUniform3fv(loc, 1, (float *)&value);
}

void Shader::setVector4(const char *name, const Vector4 &value)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
        glUniform4fv(loc, 1, (float *)&value);
}

void Shader::setMatrix3(const char *name, const Matrix3 &value)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
        glUniformMatrix3fv(loc, 1, GL_FALSE, (float *)&value);
}

void Shader::setMatrix4(const char *name, const Matrix4 &value)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
        glUniformMatrix4fv(loc, 1, GL_FALSE, (float *)&value);
}

void Shader::setBoolf(const char *format, bool value, ...)
{
    va_list args;
    va_start(args, value);
    int loc = getUniformLocationv(format, args);
    va_end(args);
    if (loc != -1)
        glUniform1i(loc, value);
}

void Shader::setFloatf(const char *format, float value, ...)
{
    va_list args;
    va_start(args, value);
    int loc = getUniformLocationv(format, args);
    va_end(args);
    if (loc != -1)
        glUniform1f(loc, value);
}

void Shader::setIntf(const char *format, int value, ...)
{
    va_list args;
    va_start(args, value);
    int loc = getUniformLocationv(format, args);
    va_end(args);
    if (loc != -1)
        glUniform1i(loc, value);
}

void Shader::setVector2f(const char *format, Vector2 value, ...)
{
    va_list args;
    va_start(args, value);
    int loc = getUniformLocationv(format, args);
    va_end(args);
    if (loc != -1)
        glUniform2fv(loc, 1, (float *)&value);
}

void Shader::setVector3f(const char *format, Vector3 value, ...)
{
    va_list args;
    va_start(args, value);
    int loc = getUniformLocationv(format, args);
    va_end(args);
    if (loc != -1)
        glUniform3fv(loc, 1, (float *)&value);
}

void Shader::setVector4f(const char *format, Vector4 value, ...)
{
    va_list args;
    va_start(args, value);
    int loc = getUniformLocationv(format, args);
    va_end(args);
    if (loc != -1)
        glUniform4fv(loc, 1, (float *)&value);
}

void Shader::setMatrix3f(const char *format, Matrix3 value, ...)
{
    va_list args;
    va_start(args, value);
    int loc = getUniformLocationv(format, args);
    va_end(args);
    if (loc != -1)
        glUniformMatrix3fv(loc, 1, GL_FALSE, (float *)&value);
}

void Shader::setMatrix4f(const char *format, Matrix4 value, ...)
{
    va_list args;
    va_start(args, value);
    int loc = getUniformLocationv(format, args);
    va_end(args);
    if (loc != -1)
        glUniformMatrix4fv(loc, 1, GL_FALSE, (float *)&value);
}

void Shader::setTexture(const char *name, GLuint texture, int unit)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(loc, unit);
    }
}

void Shader::setCubemap(const char *name, GLuint texture, int unit)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        glUniform1i(loc, unit);
    }
}

void Shader::setMaterial(const Material &material)
{
    /* Albedo */
    setTexture("uMaterial.albedo.tex", material.albedo ? material.albedo->get() : 0, 0);
    setBool("uMaterial.albedo.hasTex", material.albedo);
    setVector3("uMaterial.albedo.color", material.albedoColor);

    /* Emissive */
    setTexture("uMaterial.emissive.tex", material.emissive ? material.emissive->get() : 0, 1);
    setBool("uMaterial.emissive.hasTex", material.emissive);
    setVector3("uMaterial.emissive.color", material.emissiveColor);

    /* Normal */
    setTexture("uMaterial.normal.tex", material.normal ? material.normal->get() : 0, 2);
    setBool("uMaterial.normal.hasTex", material.normal);
    setVector3("uMaterial.normal.color", material.normal.get() ? Vector3(1.0f) : kDefaultNormal);

    /* Roughness */
    setTexture("uMaterial.roughness.tex", material.roughness ? material.roughness->get() : 0, 3);
    setBool("uMaterial.roughness.hasTex", material.roughness);
    setVector3("uMaterial.roughness.color", Vector3(material.roughnessValue));

    /* Metallic */
    setTexture("uMaterial.metallic.tex", material.metallic ? material.metallic->get() : 0, 4);
    setBool("uMaterial.metallic.hasTex", material.metallic);
    setVector3("uMaterial.metallic.color", Vector3(material.metallicValue));

    /* AO */
    setTexture("uMaterial.ao.tex", material.ao ? material.ao->get() : 0, 5);
    setBool("uMaterial.ao.hasTex", material.ao);
    setVector3("uMaterial.ao.color", Vector3(material.aoValue));
}

void Shader::setMaterial(int index, const Material &material)
{
    /* Albedo */
    setTexturef("uMaterials[%d].albedo.tex", material.albedo ? material.albedo->get() : 0, 0 + index * 6, index);
    setBoolf("uMaterials[%d].albedo.hasTex", material.albedo, index);
    setVector3f("uMaterials[%d].albedo.color", material.albedoColor, index);

    /* Emissive */
    setTexturef("uMaterials[%d].emissive.tex", material.emissive ? material.emissive->get() : 0, 1 + index * 6, index);
    setBoolf("uMaterials[%d].emissive.hasTex", material.emissive, index);
    setVector3f("uMaterials[%d].emissive.color", material.emissiveColor, index);

    /* Normal */
    setTexturef("uMaterials[%d].normal.tex", material.normal ? material.normal->get() : 0, 2 + index * 6, index);
    setBoolf("uMaterials[%d].normal.hasTex", material.normal, index);
    setVector3f("uMaterials[%d].normal.color", material.normal.get() ? Vector3(1.0f) : kDefaultNormal, index);

    /* Roughness */
    setTexturef("uMaterials[%d].roughness.tex", material.roughness ? material.roughness->get() : 0, 3 + index * 6, index);
    setBoolf("uMaterials[%d].roughness.hasTex", material.roughness, index);
    setVector3f("uMaterials[%d].roughness.color", Vector3(material.roughnessValue), index);

    /* Metallic */
    setTexturef("uMaterials[%d].metallic.tex", material.metallic ? material.metallic->get() : 0, 4 + index * 6, index);
    setBoolf("uMaterials[%d].metallic.hasTex", material.metallic, index);
    setVector3f("uMaterials[%d].metallic.color", Vector3(material.metallicValue), index);

    /* AO */
    setTexturef("uMaterials[%d].ao.tex", material.ao->get(), 5 + index * 6, index);
    setBoolf("uMaterials[%d].ao.hasTex", material.ao, index);
    setVector3f("uMaterials[%d].ao.color", Vector3(material.aoValue), index);
}

void Shader::setGbuffer(const Gbuffer *gbuffer)
{
    GLuint albedo = gbuffer->getTexture(GBUFFER_ALBEDO);
    setTexture("uGbuffer.albedo", albedo, GBUFFER_TEXTURE_UNIT(GBUFFER_ALBEDO));

    GLuint emissive = gbuffer->getTexture(GBUFFER_EMISSIVE);
    setTexture("uGbuffer.emissive", emissive, GBUFFER_TEXTURE_UNIT(GBUFFER_EMISSIVE));

    GLuint position = gbuffer->getTexture(GBUFFER_POSITION);
    setTexture("uGbuffer.position", position, GBUFFER_TEXTURE_UNIT(GBUFFER_POSITION));

    GLuint depth = gbuffer->getTexture(GBUFFER_DEPTH);
    setTexture("uGbuffer.depth", depth, GBUFFER_TEXTURE_UNIT(GBUFFER_DEPTH));

    GLuint normal = gbuffer->getTexture(GBUFFER_NORMAL);
    setTexture("uGbuffer.normal", normal, GBUFFER_TEXTURE_UNIT(GBUFFER_NORMAL));

    GLuint material = gbuffer->getTexture(GBUFFER_MATERIAL);
    setTexture("uGbuffer.material", material, GBUFFER_TEXTURE_UNIT(GBUFFER_MATERIAL));
}

void Shader::setTexturef(const char *format, GLuint texture, int unit, ...)
{
    va_list args;
    va_start(args, unit);

    int loc = getUniformLocationv(format, args);
    if (loc != -1)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(loc, unit);
    }

    va_end(args);
}

void Shader::bindUniformBlock(const char *name, GLuint bindingPoint)
{
    GLuint index = glGetUniformBlockIndex(_program, name);
    if (index != GL_INVALID_INDEX)
        glUniformBlockBinding(_program, index, bindingPoint);
}

Shader::Shader() : _program(0) {}

Shader::~Shader()
{
    if (_program)
        glDeleteProgram(_program);
}

int Shader::getUniformLocationf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int rc = getUniformLocationv(format, args);
    va_end(args);
    return rc;
}

int Shader::getUniformLocationv(const char *format, va_list args)
{
    char buffer[256];
    vsnprintf(buffer, 256, format, args);
    return glGetUniformLocation(_program, buffer);
}