/*
 Diana - A Free Meteorological Visualisation Tool

 Copyright (C) 2011 met.no

 Contact information:
 Norwegian Meteorological Institute
 Box 43 Blindern
 0313 OSLO
 NORWAY
 email: diana@met.no

 This file is part of Diana

 Diana is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 Diana is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Diana; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <QtGui>

#include "GL/gl.h"
#include "paintgl.h"

PaintGL *PaintGL::self = 0;

PaintGL::PaintGL()
{
    self = this;
    currentContext = 0;
}

PaintGL::~PaintGL()
{
    self = 0;
}

#define globalGL PaintGL::instance()
#define ctx globalGL->currentContext

PaintGLContext::PaintGLContext()
{
}

PaintGLContext::~PaintGLContext()
{
    ctx = 0;
}

void PaintGLContext::begin(QPainter *painter)
{
    // Allow this context to be implicitly made current.
    ctx = this;

    // Use the painter supplied.
    this->painter = painter;

    if (clear) {
        painter->fillRect(0, 0, painter->device()->width(), painter->device()->height(), clearColor);
        clear = false;
    }

    useTexture = false;
    blend = false;
    blendMode = QPainter::CompositionMode_Source;
    pointSize = 1.0;

    clearColor = QColor(0, 0, 0, 0);

    attributes.color = QColor(255, 255, 255, 255);
    attributes.width = 1.0;
    attributes.polygonMode[GL_FRONT] = GL_FILL;
    attributes.polygonMode[GL_BACK] = GL_FILL;

    points.clear();
    validPoints.clear();
    colors.clear();

    stack.clear();
    renderStack.clear();
    transformStack.clear();

    transform = QTransform();
}

bool PaintGLContext::isPainting() const
{
    return painter != 0;
}

void PaintGLContext::end()
{
    painter = 0;
}

void PaintGLContext::setPen()
{
    QPen pen = QPen(attributes.color, attributes.width);
    pen.setCapStyle(Qt::FlatCap);
    if (attributes.stipple && !attributes.dashes.isEmpty()) {
        /* Set the dash pattern on the pen if defined, adjusting
           the length of each element to compensate for the line
           width. */
        QVector<qreal> dashes;
        qreal width = attributes.width;
        if (width == 0)
            width = 1;

        for (int i = 0; i < attributes.dashes.size(); ++i)
            dashes << attributes.dashes[i]/width;

        pen.setDashPattern(dashes);
        pen.setDashOffset(attributes.dashOffset/width);
    }

    painter->setPen(pen);
}

void PaintGLContext::setPolygonColor(const QColor &color)
{
    switch (attributes.polygonMode[GL_FRONT]) {
    case GL_FILL:
        if (painter->testRenderHint(QPainter::Antialiasing)) {
            setPen();
        } else
            painter->setPen(Qt::NoPen);
        painter->setBrush(color);
        break;
    case GL_LINE: {
        setPen();
        painter->setBrush(Qt::NoBrush);
        break;
    }
    default:
        break;
    }

    if (blend)
        painter->setCompositionMode(blendMode);
    else
        painter->setCompositionMode(QPainter::CompositionMode_Source);
}

#define COLOR_BLEND2(a, b) QColor((a.red() + b.red())/2.0, \
                                  (a.green() + b.green())/2.0, \
                                  (a.blue() + b.blue())/2.0, \
                                  (a.alpha() + b.alpha())/2.0)

#define COLOR_BLEND4(a, b, c, d) \
    QColor((a.red() + b.red() + c.red() + d.red())/4.0, \
           (a.green() + b.green() + c.green() + d.green())/4.0, \
           (a.blue() + b.blue() + c.blue() + d.blue())/4.0, \
           (a.alpha() + b.alpha() + c.alpha() + d.alpha())/4.0)

void PaintGLContext::plotSubdivided(QPointF quad[], QColor color[], int divisions)
{
    if (divisions > 0) {
        QPointF center = (quad[0] + quad[1] + quad[2] + quad[3])/4.0;
        QColor centerColor = COLOR_BLEND4(color[0], color[1], color[2], color[3]);

        for (int j = 0; j < 4; ++j) {
            QPointF newQuad[4];
            newQuad[0] = quad[j];
            newQuad[1] = (quad[j] + quad[(j + 1) % 4])/2.0;
            newQuad[2] = center;
            newQuad[3] = (quad[j] + quad[(j + 3) % 4])/2.0;

            QColor newColor[4];
            newColor[0] = color[j];
            newColor[1] = COLOR_BLEND2(color[j], color[(j + 1) % 4]);
            newColor[2] = centerColor;
            newColor[3] = COLOR_BLEND2(color[j], color[(j + 3) % 4]);

            plotSubdivided(newQuad, newColor, divisions - 1);
        }

    } else {
        painter->setBrush(QColor((color[0].red() + color[1].red() + color[2].red() + color[3].red())/4,
                                 (color[0].green() + color[1].green() + color[2].green() + color[3].green())/4,
                                 (color[0].blue() + color[1].blue() + color[2].blue() + color[3].blue())/4,
                                 (color[0].alpha() + color[1].alpha() + color[2].alpha() + color[3].alpha())/4));
        painter->drawPolygon(quad, 4);
    }
}

void PaintGLContext::renderPrimitive()
{
    if (points.size() == 0)
        return;

    switch (mode) {
    case GL_POINTS:
        // qDebug() << "  GL_POINTS";
        setPen();
        for (int i = 0; i < points.size(); ++i) {
            painter->drawPoint(points[i]);
        }
        break;
    case GL_LINES:
        // qDebug() << "  GL_LINES";
        setPen();
        for (int i = 0; i < points.size() - 1; i += 2) {
            painter->drawLine(points[i], points[i+1]);
        }
        break;
    case GL_LINE_LOOP:
        // qDebug() << "  GL_LINE_LOOP";
        setPen();
        points.append(points[0]);
        painter->drawPolyline(QPolygonF(points));
        break;
    case GL_LINE_STRIP: {
        // qDebug() << "  GL_LINE_STRIP";
        setPen();
        painter->drawPolyline(QPolygonF(points));
        break;
    }
    case GL_TRIANGLES:
        // qDebug() << "  GL_TRIANGLES";
        setPolygonColor(attributes.color);
        for (int i = 0; i < points.size() - 2; i += 3) {
            if (validPoints[i] && validPoints[i + 1] && validPoints[i + 2]) {
                QPolygonF poly = QPolygonF(points.mid(i, 3));
                painter->drawPolygon(poly);
            }
        }
        break;
    case GL_TRIANGLE_STRIP: {
        // qDebug() << "  GL_TRIANGLE_STRIP";
        setPolygonColor(attributes.color);
        QPointF tri[3];
        tri[0] = points[0];
        tri[1] = points[1];
        for (int i = 2; i < points.size(); ++i) {
            tri[i % 3] = points[i];
            painter->drawPolygon(tri, 3);
        }
        break;
    }
    case GL_TRIANGLE_FAN: {
        // qDebug() << "  GL_TRIANGLE_FAN";
        setPolygonColor(attributes.color);
        QPointF tri[3];
        tri[0] = points[0];
        tri[1] = points[1];
        for (int i = 2; i < points.size(); ++i) {
            tri[2 - (i % 2)] = points[i];
            painter->drawPolygon(tri, 3);
        }
        break;
    }
    case GL_QUADS:
        // qDebug() << "  GL_QUADS";
        if (!blend)
            setPolygonColor(attributes.color);
        else {
            // Optimisation: Diana only ever draws filled, blended quads without edges.
            painter->setPen(Qt::NoPen);
        }

        for (int i = 0; i < points.size() - 3; i += 4) {
            if (!validPoints[i] || !validPoints[i] || !validPoints[i] || !validPoints[i])
                continue;
            QPolygonF poly = QPolygonF(points.mid(i, 4));
            if (useTexture) {
                QImage texture = textures[currentTexture];
                QTransform t;
                QPolygonF source(QRectF(texture.rect()));
                if (QTransform::quadToQuad(source, poly, t)) {
                    painter->save();
                    // No need to record this transformation.
                    painter->setTransform(transform * t);
                    painter->drawImage(0, 0, texture);
                    painter->restore();
                }
            } else {
                if (blend) {
                    // Optimisation: Diana only ever draws filled, blended quads without edges.
                    painter->setBrush(colors[i]);
                }
                painter->drawPolygon(poly);
            }
        }
        break;
    case GL_QUAD_STRIP: {
        // qDebug() << "  GL_QUAD_STRIP";
        if (!blend)
            setPolygonColor(attributes.color);

        QPointF quad[4];
        quad[0] = points[0];
        quad[1] = points[1];
        QColor color[4];
        color[0] = colors[0];
        color[1] = colors[1];

        for (int i = 2; i < points.size() - 1; i += 2) {

            if (i % 4 == 2) {
                quad[i % 4] = points[i + 1];
                quad[i % 4 + 1] = points[i];
                color[i % 4] = colors[i + 1];
                color[i % 4 + 1] = colors[i];
            } else {
                quad[i % 4] = points[i];
                quad[i % 4 + 1] = points[i + 1];
                color[i % 4] = colors[i];
                color[i % 4 + 1] = colors[i + 1];
            }

            if (validPoints[i - 2] && validPoints[i - 1] && validPoints[i] && validPoints[i + 1]) {
                if (blend) {
                    if (smooth) {
                        painter->setPen(Qt::NoPen);
                        plotSubdivided(quad, color);
                    } else {
                        setPolygonColor(colors[i]);
                        painter->drawPolygon(quad, 4);
                    }
                } else
                    painter->drawPolygon(quad, 4);
            }
        }
        break;
    }
    case GL_POLYGON: {
        // qDebug() << "  GL_POLYGON";
        setPolygonColor(attributes.color);
        QPolygonF poly;
        for (int i = 0; i < points.size(); ++i) {
            if (validPoints[i])
                poly.append(points[i]);
        }
        painter->drawPolygon(poly);
        break;
    }
    default:
        break;
    }

    points.clear();
    validPoints.clear();
    colors.clear();
}

void PaintGLContext::setViewportTransform()
{
    QTransform t;
    t = transform.translate(viewport.left(), viewport.top());
    t = t.scale(viewport.width()/window.width(), viewport.height()/window.height());
    t = t.translate(-window.left(), -window.top());
    transform = t;
}

#define ENSURE_CTX if (!globalGL || !ctx) return;
#define ENSURE_CTX_BOOL if (!globalGL || !ctx) return false;
#define ENSURE_CTX_AND_PAINTER if (!globalGL || !ctx || !ctx->painter) return;
#define ENSURE_CTX_AND_PAINTER_BOOL if (!globalGL || !ctx || !ctx->painter) return false;

void glBegin(GLenum mode)
{
    ENSURE_CTX
    ctx->stack.push(ctx->mode);
    ctx->mode = mode;
}

void glBindTexture(GLenum target, GLuint texture)
{
    ENSURE_CTX

    // Assume target == GL_TEXTURE_2D
    ctx->currentTexture = texture;
}

void glBitmap(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig,
              GLfloat xmove, GLfloat ymove, const GLubyte *bitmap)
{
    ENSURE_CTX

    // Diana only uses this to displace bitmaps.
    ctx->bitmapMove = ctx->transform * QPointF(xmove, ymove);
}

void glBlendFunc(GLenum sfactor, GLenum dfactor)
{
    ENSURE_CTX_AND_PAINTER
    if (sfactor == GL_SRC_ALPHA && dfactor == GL_ONE_MINUS_SRC_ALPHA)
        ctx->blendMode = QPainter::CompositionMode_SourceOver;
}

void glCallList(GLuint list)
{
    ENSURE_CTX_AND_PAINTER
    QPicture picture = ctx->lists[list];
    ctx->painter->drawPicture(0, 0, picture);
}

void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    ENSURE_CTX
    ctx->clearColor = QColor(red * 255, green * 255, blue * 255, alpha * 255);
}

void glClear(GLbitfield mask)
{
    ENSURE_CTX

    if (mask & GL_COLOR_BUFFER_BIT) {
        if (ctx->isPainting())
            ctx->painter->fillRect(0, 0, ctx->painter->device()->width(),
                                         ctx->painter->device()->height(), ctx->clearColor);
        else
            ctx->clear = true;
    }
}

void glClearStencil(GLint s)
{
}

void glColor3d(GLdouble red, GLdouble green, GLdouble blue)
{
    ENSURE_CTX
    ctx->attributes.color = QColor(red * 255, green * 255, blue * 255);
    if (ctx->painter && !ctx->blend)
        ctx->renderPrimitive();
}

void glColor3f(GLfloat red, GLfloat green, GLfloat blue)
{
    ENSURE_CTX
    ctx->attributes.color = QColor(red * 255, green * 255, blue * 255);
    if (ctx->painter && !ctx->blend)
        ctx->renderPrimitive();
}

void glColor3fv(const GLfloat *v)
{
    ENSURE_CTX
    ctx->attributes.color = QColor(v[0] * 255, v[1] * 255, v[2] * 255);
    if (ctx->painter && !ctx->blend)
        ctx->renderPrimitive();
}

void glColor3ub(GLubyte red, GLubyte green, GLubyte blue)
{
    ENSURE_CTX
    ctx->attributes.color = QColor(red, green, blue);
    if (ctx->painter && !ctx->blend)
        ctx->renderPrimitive();
}

void glColor3ubv(const GLubyte *v)
{
    ENSURE_CTX
    ctx->attributes.color = QColor(v[0], v[1], v[2]);
    if (ctx->painter && !ctx->blend)
        ctx->renderPrimitive();
}

void glColor4d(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha)
{
    ENSURE_CTX
    ctx->attributes.color = QColor(red * 255, green * 255, blue * 255, alpha * 255);
    if (ctx->painter && !ctx->blend)
        ctx->renderPrimitive();
}

void glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    ENSURE_CTX
    ctx->attributes.color = QColor(red * 255, green * 255, blue * 255, alpha * 255);
    if (ctx->painter && !ctx->blend)
        ctx->renderPrimitive();
}

void glColor4ub(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha)
{
    ENSURE_CTX
    ctx->attributes.color = QColor(red, green, blue, alpha);
    if (ctx->painter && !ctx->blend)
        ctx->renderPrimitive();
}

void glColor4fv(const GLfloat *v)
{
    ENSURE_CTX
    ctx->attributes.color = QColor(v[0] * 255, v[1] * 255, v[2] * 255, v[3] * 255);
    if (ctx->painter && !ctx->blend)
        ctx->renderPrimitive();
}

void glColor4ubv(const GLubyte *v)
{
    ENSURE_CTX
    ctx->attributes.color = QColor(v[0], v[1], v[2], v[3]);
    if (ctx->painter && !ctx->blend)
        ctx->renderPrimitive();
}

void glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
}

void glDeleteLists(GLuint list, GLsizei range)
{
    ENSURE_CTX
    for (GLuint i = list; i < list + range; ++i)
        ctx->lists.remove(i - 1);
}

void glDeleteTextures(GLsizei n, const GLuint *textures)
{
}

void glDisable(GLenum cap)
{
    ENSURE_CTX
    switch (cap) {
    case GL_BLEND:
        ctx->blend = false;
        break;
    case GL_TEXTURE_2D:
        ctx->useTexture = false;
        break;
    case GL_LINE_STIPPLE:
        ctx->attributes.stipple = false;
        break;
    default:
        break;
    }
}

void glDisableClientState(GLenum cap)
{
}

void glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
}

void glDrawBuffer(GLenum mode)
{
}

void glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type,
                  const GLvoid *pixels)
{
    ENSURE_CTX_AND_PAINTER
    // Assuming type == GL_UNSIGNED_BYTE

    int sx = ctx->pixelStore[GL_UNPACK_SKIP_PIXELS];
    int sy = ctx->pixelStore[GL_UNPACK_SKIP_ROWS];
    int sr = ctx->pixelStore[GL_UNPACK_ROW_LENGTH];

    QImage image = QImage((const uchar *)pixels + (sr * 4 * sy) + (sx * 4), width, height, sr * 4, QImage::Format_ARGB32).rgbSwapped();

    QPointF pos = ctx->rasterPos + ctx->bitmapMove;

    ctx->painter->save();
    // No need to record this transformation.
    ctx->painter->resetTransform();
    ctx->painter->translate(pos);
    ctx->painter->scale(ctx->pixelZoom.x(), -ctx->pixelZoom.y());
    ctx->painter->drawImage(0, 0, image);
    ctx->painter->restore();
}

void glEdgeFlag(GLboolean flag)
{
}

void glEnable(GLenum cap)
{
    ENSURE_CTX_AND_PAINTER
    switch (cap) {
    case GL_BLEND:
        ctx->blend = true;
        break;
    case GL_TEXTURE_2D:
        ctx->useTexture = true;
        break;
    case GL_LINE_STIPPLE:
        ctx->attributes.stipple = true;
        break;
    default:
        break;
    }
}

void glEnableClientState(GLenum cap)
{
}

void glEnd()
{
    ENSURE_CTX_AND_PAINTER
    ctx->renderPrimitive();
    ctx->mode = ctx->stack.pop();
}

void glEndList()
{
    ENSURE_CTX
    RenderItem item = ctx->renderStack.pop();
    ctx->painter->end();
    delete ctx->painter;

    ctx->painter = item.painter;

    if (item.mode == GL_COMPILE_AND_EXECUTE)
        glCallList(item.list);
}

void glFlush()
{
    ENSURE_CTX_AND_PAINTER
    ctx->renderPrimitive();
}

GLuint glGenLists(GLsizei range)
{
    if (!ctx)
        return 0;

    // Diana only ever asks for one list at a time, so we can do something simple.
    GLuint next = 1;

    while (ctx->lists.contains(next))
        ++next;

    ctx->lists[next] = QPicture();
    return next;
}

void glGenTextures(GLsizei n, GLuint *textures)
{
    ENSURE_CTX

    GLuint min = ctx->textures.keys()[0];
    foreach (GLuint i, ctx->textures.keys()) {
        if (i < min) min = i;
    }
    if (min >= (GLuint)n) {
        for (GLuint i = 0; i < (GLuint)n; ++i) {
            ctx->textures[i] = QImage();
            textures[i] = i + 1;
        }
        return;
    }
    GLuint max = min;
    foreach (GLuint i, ctx->textures.keys()) {
        if (i > max) max = i;
    }
    for (GLuint i = 0; i < (GLuint)n; ++i) {
        ctx->textures[max + 1 + i] = QImage();
        textures[i] = i + 1;
    }
}

void glGetFloatv(GLenum pname, GLfloat *params)
{
    ENSURE_CTX
    switch (pname) {
    case GL_CURRENT_COLOR:
    {
        QColor c = ctx->attributes.color;
        params[0] = c.red()/255.0;
        params[1] = c.green()/255.0;
        params[2] = c.blue()/255.0;
        params[3] = c.alpha()/255.0;
        break;
    }
    case GL_LINE_WIDTH:
        params[0] = ctx->attributes.width;
        break;
    default:
        break;
    }
}

void glGetIntegerv(GLenum pname, GLint *params)
{
    ENSURE_CTX_AND_PAINTER
    switch (pname) {
    case GL_VIEWPORT:
        params[0] = ctx->painter->window().left();
        params[1] = ctx->painter->window().top();
        params[2] = ctx->painter->window().width();
        params[3] = ctx->painter->window().height();
        break;
    case GL_MAX_VIEWPORT_DIMS:
        params[0] = 65535;
        params[1] = 65535;
        break;
    default:
        break;
    }
}

void glIndexi(GLint c)
{
    // Unimplemented
}

GLboolean glIsList(GLuint list)
{
    if (!ctx)
        return 0;

    if (ctx->lists.contains(list))
        return 1;
    else
        return 0;
}

void glLineStipple(GLint factor, GLushort pattern)
{
    ENSURE_CTX

    QVector<qreal> dashes;
    GLushort state = pattern & 1;
    bool gapStart = (pattern & 1) == 0;
    int number = 0;
    int total = 0;

    for (int i = 0; i < 16; ++i) {
        GLushort dash = pattern & 1;
        if (dash == state)
            number++;
        else {
            dashes << number * factor;
            total += number * factor;
            state = dash;
            number = 1;
        }
        pattern = pattern >> 1;
    }
    if (number > 0)
        dashes << number * factor;

    // Ensure that the pattern has an even number of elements by inserting
    // a zero-size element if necessary.
    if (dashes.size() % 2 == 1)
        dashes << 0;

    /* If the pattern starts with a gap then move it to the end of the
       vector and adjust the starting offset to its location to ensure
       that it appears at the start of the pattern. (This is because
       QPainter's dash pattern rendering assumes that the first element
       is a line. */
    if (gapStart) {
        dashes << dashes.first();
        dashes.pop_front();
        ctx->attributes.dashOffset = total - dashes.last();
    } else
        ctx->attributes.dashOffset = 0;
    ctx->attributes.dashes = dashes;
}

void glLineWidth(GLfloat width)
{
    ENSURE_CTX_AND_PAINTER
    ctx->attributes.width = width;
}

void glLoadIdentity()
{
    ENSURE_CTX_AND_PAINTER
    ctx->transform = QTransform();
}

void glNewList(GLuint list, GLenum mode)
{
    ENSURE_CTX

    RenderItem item;
    item.mode = mode;
    item.painter = ctx->painter;
    item.list = list;
    ctx->renderStack.push(item);

    ctx->painter = new QPainter;
    ctx->painter->begin(&ctx->lists[list]);
    if (item.painter)
        ctx->painter->setRenderHints(item.painter->renderHints());
}

void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top,
             GLdouble near_val, GLdouble far_val)
{
    ENSURE_CTX

    ctx->window = QRectF(left, top, right - left, bottom - top);
    ctx->setViewportTransform();
}

void glPixelStorei(GLenum pname, GLint param)
{
    ENSURE_CTX
    ctx->pixelStore[pname] = param;
}

void glPixelZoom(GLfloat xfactor, GLfloat yfactor)
{
    ENSURE_CTX
    ctx->pixelZoom = QPointF(xfactor, yfactor);
}

void glPointSize(GLfloat size)
{
    ENSURE_CTX
    ctx->pointSize = size;
}

void glPolygonMode(GLenum face, GLenum mode)
{
    ENSURE_CTX

    if (face == GL_FRONT_AND_BACK) {
        glPolygonMode(GL_FRONT, mode);
        glPolygonMode(GL_BACK, mode);
        return;
    }

    switch (mode) {
    case GL_FILL:
        ctx->attributes.polygonMode[face] = GL_FILL;
        break;
    case GL_LINE:
        ctx->attributes.polygonMode[face] = GL_LINE;
        break;
    default:
        break;
    }
}

void glPolygonStipple(const GLubyte *mask)
{
}

void glPopMatrix()
{
    ENSURE_CTX
    ctx->transform = ctx->transformStack.pop();
}

void glPushMatrix()
{
    ENSURE_CTX
    ctx->transformStack.push(ctx->transform);
}

void glRasterPos2f(GLfloat x, GLfloat y)
{
    ENSURE_CTX
    ctx->rasterPos = ctx->transform * QPointF(x, y);
}

void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height,
                  GLenum format, GLenum type, GLvoid *pixels)
{
}

void glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    glBegin(GL_POLYGON);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}

void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    ENSURE_CTX

    // Diana only rotates about the z axis.
    ctx->transform = ctx->transform.rotate(angle);
}

void glScalef(GLfloat x, GLfloat y, GLfloat z)
{
    Q_UNUSED(z)
    ENSURE_CTX
    ctx->transform = ctx->transform.scale(x, y);
}

void glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
}

void glShadeModel(GLenum mode)
{
    switch (mode) {
    case GL_FLAT:
        ctx->smooth = false;
        break;
    case GL_SMOOTH:
    default:
        ctx->smooth = true;
        break;
    }
}

void glStencilFunc(GLenum func, GLint ref, GLuint mask)
{
}

void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
}

void glTexCoord2f(GLfloat s, GLfloat t)
{
    ENSURE_CTX
}

void glTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
}

void glTexImage2D(GLenum target, GLint level, GLint internalFormat,
                  GLsizei width, GLsizei height, GLint border,
                  GLenum format, GLenum type, const GLvoid *pixels)
{
    ENSURE_CTX

    QImage texture = QImage(width, height, QImage::Format_ARGB32);
    // Assuming type == GL_UNSIGNED_BYTE
    int length = width * height;
    texture.loadFromData((const uchar *)pixels, length);
    ctx->textures[ctx->currentTexture] = texture;
}

void glTexParameteri(GLenum target, GLenum pname, GLint param)
{
}

void glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
    Q_UNUSED(z)
    ENSURE_CTX
    ctx->transform = ctx->transform.translate(x, y);
}

void glVertex2dv(GLdouble *v)
{
    ENSURE_CTX
    QPointF p = ctx->transform * QPointF(v[0], v[1]);
    ctx->points.append(p);
    ctx->validPoints.append(!isnan(p.x()) && !isnan(p.y()));
    ctx->colors.append(ctx->attributes.color);
}

void glVertex2f(GLfloat x, GLfloat y)
{
    ENSURE_CTX
    QPointF p = ctx->transform * QPointF(x, y);
    ctx->points.append(p);
    ctx->validPoints.append(!isnan(p.x()) && !isnan(p.y()));
    ctx->colors.append(ctx->attributes.color);
}

void glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
    Q_UNUSED(z)
    ENSURE_CTX
    QPointF p = ctx->transform * QPointF(x, y);
    ctx->points.append(p);
    ctx->validPoints.append(!isnan(p.x()) && !isnan(p.y()));
    ctx->colors.append(ctx->attributes.color);
}

void glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid *ptr)
{
}

void glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    ENSURE_CTX
    ctx->viewport = QRect(x, y, width, height);
}



bool glText::testDefineFonts(std::string path)
{
    return true;
}

bool glText::defineFonts(const std::string pattern, const std::string family, const std::string psname)
{
    return true;
}

bool glText::defineFont(const std::string, const std::string, const glText::FontFace, const int, const std::string, const float, const float)
{
    return true;
}

bool glText::set(const std::string name, const glText::FontFace face, const float size)
{
    ENSURE_CTX_BOOL

    setFont(name);
    setFontFace(face);
    setFontSize(size);
    return true;
}

bool glText::setFont(const std::string name)
{
    ENSURE_CTX_BOOL

    ctx->font.setFamily(QString::fromStdString(name));
    return true;
}

bool glText::setFontFace(const glText::FontFace face)
{
    ENSURE_CTX_BOOL

    if (face & 1)
        ctx->font.setWeight(QFont::Bold);
    if (face & 2)
        ctx->font.setItalic(true);
    return true;
}

bool glText::setFontSize(const float size)
{
    ENSURE_CTX_BOOL

    ctx->font.setPointSizeF(size);
    return true;
}

bool glText::drawChar(const int c, const float x, const float y,
                      const float a)
{
    ENSURE_CTX_AND_PAINTER_BOOL

    QPointF sp = ctx->transform * QPointF(x, y);

    ctx->painter->setFont(ctx->font);
    ctx->painter->save();
    // No need to record this transformation.
    ctx->painter->resetTransform();
    ctx->painter->translate(sp);
    ctx->painter->rotate(a);
    ctx->painter->setPen(QPen(ctx->attributes.color));
    ctx->painter->drawText(0, 0, QChar(c));
    ctx->painter->restore();
    return true;
}

bool glText::drawStr(const char* s, const float x, const float y,
                     const float a)
{
    ENSURE_CTX_AND_PAINTER_BOOL

    QPointF sp = ctx->transform * QPointF(x, y);

    ctx->painter->setFont(ctx->font);
    ctx->painter->save();
    // No need to record this transformation.
    ctx->painter->resetTransform();
    ctx->painter->translate(sp);
    ctx->painter->rotate(-a);
    ctx->painter->setPen(QPen(ctx->attributes.color));
    ctx->painter->drawText(0, 0, s);
    ctx->painter->restore();
    return true;
}

bool glText::getCharSize(const int c, float& w, float& h)
{
    ENSURE_CTX_AND_PAINTER_BOOL

    QFontMetricsF fm(ctx->font);
    QRectF rect = ctx->transform.inverted().mapRect(fm.boundingRect(QChar(c)));
    w = rect.width();
    h = rect.height();
    return true;
}

bool glText::getMaxCharSize(float& w, float& h)
{
    ENSURE_CTX_AND_PAINTER_BOOL

    QFontMetricsF fm(ctx->font);
    QPointF p = QPointF(fm.maxWidth(), fm.height()) * ctx->transform.inverted();
    w = p.x();
    h = p.y();
    return true;
}

bool glText::getStringSize(const char* s, float& w, float& h)
{
    ENSURE_CTX_AND_PAINTER_BOOL

    QFontMetricsF fm(ctx->font);
    QRectF rect = ctx->transform.inverted().mapRect(fm.boundingRect(s));
    w = rect.width();
    h = rect.height();
    return true;
}

PaintGLWidget::PaintGLWidget(QWidget *parent, bool antialiasing)
    : QWidget(parent), initialized(false), antialiasing(antialiasing)
{
    glContext = new PaintGLContext();
}

PaintGLWidget::~PaintGLWidget()
{
    delete glContext;
}

QImage PaintGLWidget::grabFrameBuffer(bool withAlpha)
{
    makeCurrent();
    if (!initialized) {
        initializeGL();
        initialized = true;
    }

    QImage image;
    if (withAlpha)
        image = QImage(size(), QImage::Format_ARGB32);
    else
        image = QImage(size(), QImage::Format_RGB32);

    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    paint(&painter);
    painter.end();

    return image;
}

bool PaintGLWidget::isValid()
{
    return true;
}

void PaintGLWidget::makeCurrent()
{
    ctx = glContext;
}

void PaintGLWidget::swapBuffers()
{
    if (!glContext->isPainting())
        update();
}

void PaintGLWidget::updateGL()
{
    update();
}

/**
  * This function is intended to be implemented in a subclass.
  */
void PaintGLWidget::initializeGL()
{
}

/**
  * This function is intended to be implemented in a subclass.
  */
void PaintGLWidget::paintGL()
{
}

/**
  * This function is intended to be implemented in a subclass.
  */
void PaintGLWidget::resizeGL(int width, int height)
{
}

void PaintGLWidget::paintEvent(QPaintEvent* event)
{
    makeCurrent();
    if (!initialized) {
        initializeGL();
        initialized = true;
    }

    QPainter painter;
    painter.begin(this);
    glContext->begin(&painter);
    if (antialiasing)
        painter.setRenderHint(QPainter::Antialiasing);

    paintGL();

    glContext->end();
    painter.end();
}

void PaintGLWidget::resizeEvent(QResizeEvent* event)
{
    makeCurrent();

    int w = event->size().width();
    int h = event->size().height();

    resizeGL(w, h);
}

void PaintGLWidget::setAutoBufferSwap(bool enable)
{
    Q_UNUSED(enable)
}

void PaintGLWidget::paint(QPainter *painter)
{
    glContext->begin(painter);
    paintGL();
    glContext->end();
}