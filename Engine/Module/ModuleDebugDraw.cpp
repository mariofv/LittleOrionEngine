#include "ModuleDebugDraw.h"
#include "Application.h"
#include "ModuleProgram.h"
#include "Component/ComponentCamera.h"

#define DEBUG_DRAW_IMPLEMENTATION
#include "UI/DebugDraw.h"     // Debug Draw API. Notice that we need the DEBUG_DRAW_IMPLEMENTATION macro here!

#include "GL/glew.h"
#include <assert.h>

class IDebugDrawOpenGLImplementation final : public dd::RenderInterface
{
public:

    //
    // dd::RenderInterface overrides:
    //

    void drawPointList(const dd::DrawVertex* points, int count, bool depthEnabled) override
    {
        assert(points != nullptr);
        assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);
		GLuint shader_program = App->program->linepoint_program;
        glBindVertexArray(linePointVAO);
        glUseProgram(shader_program);

        glUniformMatrix4fv(
            glGetUniformLocation(shader_program, "u_MvpMatrix"),
            1, GL_TRUE, reinterpret_cast<const float*>(&mvpMatrix)
        );

        bool already = glIsEnabled(GL_DEPTH_TEST);

        if (depthEnabled)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        // NOTE: Could also use glBufferData to take advantage of the buffer orphaning trick...
        glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), points);

        // Issue the draw call:
        glDrawArrays(GL_POINTS, 0, count);

        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        checkGLError(__FILE__, __LINE__);

        if (already)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

    }

    void drawLineList(const dd::DrawVertex* lines, int count, bool depthEnabled) override
    {
        assert(lines != nullptr);
        assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

		GLuint shader_program = App->program->linepoint_program;
        glBindVertexArray(linePointVAO);
        glUseProgram(shader_program);

        glUniformMatrix4fv(
            glGetUniformLocation(shader_program, "u_MvpMatrix"),
            1, GL_TRUE, reinterpret_cast<const float*>(&mvpMatrix)
        );

        bool already = glIsEnabled(GL_DEPTH_TEST);

        if (depthEnabled)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        // NOTE: Could also use glBufferData to take advantage of the buffer orphaning trick...
        glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), lines);

        // Issue the draw call:
        glDrawArrays(GL_LINES, 0, count);

        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        checkGLError(__FILE__, __LINE__);

        if (already)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

    }

    void drawGlyphList(const dd::DrawVertex* glyphs, int count, dd::GlyphTextureHandle glyphTex) override
    {
        assert(glyphs != nullptr);
        assert(count > 0 && count <= DEBUG_DRAW_VERTEX_BUFFER_SIZE);

		GLuint shader_program = App->program->text_program;
        glBindVertexArray(textVAO);
        glUseProgram(shader_program);

        // These doesn't have to be reset every draw call, I'm just being lazy ;)
        glUniform1i(
            glGetUniformLocation(shader_program, "u_glyphTexture"),
            0
        );
        glUniform2f(
            glGetUniformLocation(shader_program, "u_screenDimensions"),
            static_cast<GLfloat>(width),
            static_cast<GLfloat>(height)
        );

        if (glyphTex != nullptr)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, handleToGL(glyphTex));
        }

        bool already_blend = glIsEnabled(GL_BLEND);

        if (!already_blend)
        {
            glEnable(GL_BLEND);
        }

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        bool already = glIsEnabled(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_TEST);

        glBindBuffer(GL_ARRAY_BUFFER, textVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(dd::DrawVertex), glyphs);

        glDrawArrays(GL_TRIANGLES, 0, count); // Issue the draw call

        if (!already_blend)
        {
            glDisable(GL_BLEND);
        }

        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        checkGLError(__FILE__, __LINE__);

        if (already)
        {
            glEnable(GL_DEPTH_TEST);
        }
    }

    dd::GlyphTextureHandle createGlyphTexture(int width, int height, const void* pixels) override
    {
        assert(width > 0 && height > 0);
        assert(pixels != nullptr);

        GLuint textureId = 0;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        checkGLError(__FILE__, __LINE__);

        return GLToHandle(textureId);
    }

    void destroyGlyphTexture(dd::GlyphTextureHandle glyphTex) override
    {
        if (glyphTex == nullptr)
        {
            return;
        }

        const GLuint textureId = handleToGL(glyphTex);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &textureId);
    }

    // These two can also be implemented to perform GL render
    // state setup/cleanup, but we don't use them in this sample.
    //void beginDraw() override { }
    //void endDraw()   override { }

    //
    // Local methods:
    //

    IDebugDrawOpenGLImplementation()
        : mvpMatrix()
        , width(0)
        , height(0)
        , linePointVAO(0)
        , linePointVBO(0)
        , textVAO(0)
        , textVBO(0)
    {
        //std::printf("\n");
        //std::printf("GL_VENDOR    : %s\n",   glGetString(GL_VENDOR));
        //std::printf("GL_RENDERER  : %s\n",   glGetString(GL_RENDERER));
        //std::printf("GL_VERSION   : %s\n",   glGetString(GL_VERSION));
        //std::printf("GLSL_VERSION : %s\n\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        //std::printf("IDebugDrawOpenGLImplementation initializing ...\n");

        // Default OpenGL states:
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        // This has to be enabled since the point drawing shader will use gl_PointSize.
        glEnable(GL_PROGRAM_POINT_SIZE);

        setupVertexBuffers();

        //std::printf("IDebugDrawOpenGLImplementation ready!\n\n");
    }

    ~IDebugDrawOpenGLImplementation()
    {
        glDeleteVertexArrays(1, &linePointVAO);
        glDeleteBuffers(1, &linePointVBO);

        glDeleteVertexArrays(1, &textVAO);
        glDeleteBuffers(1, &textVBO);
    }

    void setupVertexBuffers()
    {
        //std::printf("> IDebugDrawOpenGLImplementation::setupVertexBuffers()\n");

        //
        // Lines/points vertex buffer:
        //
        {
            glGenVertexArrays(1, &linePointVAO);
            glGenBuffers(1, &linePointVBO);
            checkGLError(__FILE__, __LINE__);

            glBindVertexArray(linePointVAO);
            glBindBuffer(GL_ARRAY_BUFFER, linePointVBO);

            // RenderInterface will never be called with a batch larger than
            // DEBUG_DRAW_VERTEX_BUFFER_SIZE vertexes, so we can allocate the same amount here.
            glBufferData(GL_ARRAY_BUFFER, DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), nullptr, GL_STREAM_DRAW);
            checkGLError(__FILE__, __LINE__);

            // Set the vertex format expected by 3D points and lines:
            std::size_t offset = 0;

            glEnableVertexAttribArray(0); // in_Position (vec3)
            glVertexAttribPointer(
                /* index     = */ 0,
                /* size      = */ 3,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));
            offset += sizeof(float) * 3;

            glEnableVertexAttribArray(1); // in_ColorPointSize (vec4)
            glVertexAttribPointer(
                /* index     = */ 1,
                /* size      = */ 4,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));

            checkGLError(__FILE__, __LINE__);

            // VAOs can be a pain in the neck if left enabled...
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        //
        // Text rendering vertex buffer:
        //
        {
            glGenVertexArrays(1, &textVAO);
            glGenBuffers(1, &textVBO);
            checkGLError(__FILE__, __LINE__);

            glBindVertexArray(textVAO);
            glBindBuffer(GL_ARRAY_BUFFER, textVBO);

            // NOTE: A more optimized implementation might consider combining
            // both the lines/points and text buffers to save some memory!
            glBufferData(GL_ARRAY_BUFFER, DEBUG_DRAW_VERTEX_BUFFER_SIZE * sizeof(dd::DrawVertex), nullptr, GL_STREAM_DRAW);
            checkGLError(__FILE__, __LINE__);

            // Set the vertex format expected by the 2D text:
            std::size_t offset = 0;

            glEnableVertexAttribArray(0); // in_Position (vec2)
            glVertexAttribPointer(
                /* index     = */ 0,
                /* size      = */ 2,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));
            offset += sizeof(float) * 2;

            glEnableVertexAttribArray(1); // in_TexCoords (vec2)
            glVertexAttribPointer(
                /* index     = */ 1,
                /* size      = */ 2,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));
            offset += sizeof(float) * 2;

            glEnableVertexAttribArray(2); // in_Color (vec4)
            glVertexAttribPointer(
                /* index     = */ 2,
                /* size      = */ 4,
                /* type      = */ GL_FLOAT,
                /* normalize = */ GL_FALSE,
                /* stride    = */ sizeof(dd::DrawVertex),
                /* offset    = */ reinterpret_cast<void*>(offset));

            checkGLError(__FILE__, __LINE__);

            // Ditto.
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    static GLuint handleToGL(dd::GlyphTextureHandle handle)
    {
        const std::size_t temp = reinterpret_cast<std::size_t>(handle);
        return static_cast<GLuint>(temp);
    }

    static dd::GlyphTextureHandle GLToHandle(const GLuint id)
    {
        const std::size_t temp = static_cast<std::size_t>(id);
        return reinterpret_cast<dd::GlyphTextureHandle>(temp);
    }

    static void checkGLError(const char* file, int line)
    {
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            //errorF("%s(%d) : GL_CORE_ERROR=0x%X - %s", file, line, err, errorToString(err));
        }
    }

    // The "model-view-projection" matrix for the scene.
    // In this demo, it consists of the camera's view and projection matrices only.
    math::float4x4 mvpMatrix;
    unsigned width, height;

private:
    GLuint linePointVAO;
    GLuint linePointVBO;

    GLuint textVAO;
    GLuint textVBO;
}; // class IDebugDrawOpenGLImplementation


IDebugDrawOpenGLImplementation* ModuleDebugDraw::dd_interface_implementation = 0; // TODO: Ask why this is needed

// Called before render is available
bool ModuleDebugDraw::Init()
{
	APP_LOG_SECTION("************ Module Debug Draw Init ************");

	dd_interface_implementation = new IDebugDrawOpenGLImplementation();
    dd::initialize(dd_interface_implementation);
    
    APP_LOG_SUCCESS("Module Debug Draw initialized correctly.")

	return true;
}

void ModuleDebugDraw::Render(const ComponentCamera& camera)
{
    math::float4x4 view = camera.GetViewMatrix();
    math::float4x4 proj = camera.GetProjectionMatrix();

    dd_interface_implementation->width = camera.GetWidth();
    dd_interface_implementation->height = camera.GetHeigt();
    dd_interface_implementation->mvpMatrix = proj * view;

    dd::flush();
}

// Called before quitting
bool ModuleDebugDraw::CleanUp()
{
	APP_LOG_INFO("Destroying Module Debug Draw");

    dd::shutdown();

    delete dd_interface_implementation;
    dd_interface_implementation = 0;

	return true;
}