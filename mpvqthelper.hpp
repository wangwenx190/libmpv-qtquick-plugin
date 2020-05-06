/* Copyright (C) 2017 the mpv developers
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

// Don't use any deprecated APIs from MPV.
#ifdef MPV_ENABLE_DEPRECATED
#undef MPV_ENABLE_DEPRECATED
#endif

#define MPV_ENABLE_DEPRECATED 0

#include <mpv/client.h>
#include <mpv/render_gl.h>

/**
 * Note: these helpers are provided for convenience for C++/Qt applications.
 * This is based on the public API in client.h, and it does not encode any
 * knowledge that is not known or guaranteed outside of the C client API. You
 * can even copy and modify this code as you like, or implement similar things
 * for other languages.
 */

#ifdef WWX190_DYNAMIC_LIBMPV
#include <QDebug>
#include <QFileInfo>
#include <QLibrary>
#endif
#include <QVariant>

namespace mpv {

namespace qt {

#ifdef WWX190_DYNAMIC_LIBMPV
using lp_mpv_get_property = int (*)(mpv_handle *, const char *, mpv_format,
                                    void *);
using lp_mpv_set_property = int (*)(mpv_handle *, const char *, mpv_format,
                                    void *);
using lp_mpv_set_property_async = int (*)(mpv_handle *, uint64_t, const char *,
                                          mpv_format, void *);
using lp_mpv_command_node = int (*)(mpv_handle *, mpv_node *, mpv_node *);
using lp_mpv_command_node_async = int (*)(mpv_handle *, uint64_t, mpv_node *);
using lp_mpv_load_config_file = int (*)(mpv_handle *, const char *);
using lp_mpv_error_string = const char *(*)(int);
using lp_mpv_observe_property = int (*)(mpv_handle *, uint64_t, const char *,
                                        mpv_format);
using lp_mpv_render_context_create = int (*)(mpv_render_context **,
                                             mpv_handle *, mpv_render_param *);
using lp_mpv_render_context_set_update_callback = void (*)(mpv_render_context *,
                                                           mpv_render_update_fn,
                                                           void *);
using lp_mpv_render_context_render = int (*)(mpv_render_context *,
                                             mpv_render_param *);
using lp_mpv_set_wakeup_callback = void (*)(mpv_handle *, void (*)(void *),
                                            void *);
using lp_mpv_initialize = int (*)(mpv_handle *);
using lp_mpv_render_context_free = void (*)(mpv_render_context *);
using lp_mpv_terminate_destroy = void (*)(mpv_handle *);
using lp_mpv_request_log_messages = int (*)(mpv_handle *, const char *);
using lp_mpv_wait_event = mpv_event *(*)(mpv_handle *, double);
using lp_mpv_create = mpv_handle *(*)();
using lp_mpv_event_name = const char *(*)(mpv_event_id);
using lp_mpv_free_node_contents = void (*)(mpv_node *);

static lp_mpv_get_property m_lp_mpv_get_property = nullptr;
static lp_mpv_set_property m_lp_mpv_set_property = nullptr;
static lp_mpv_set_property_async m_lp_mpv_set_property_async = nullptr;
static lp_mpv_command_node m_lp_mpv_command_node = nullptr;
static lp_mpv_command_node_async m_lp_mpv_command_node_async = nullptr;
static lp_mpv_load_config_file m_lp_mpv_load_config_file = nullptr;
static lp_mpv_error_string m_lp_mpv_error_string = nullptr;
static lp_mpv_observe_property m_lp_mpv_observe_property = nullptr;
static lp_mpv_render_context_create m_lp_mpv_render_context_create = nullptr;
static lp_mpv_render_context_set_update_callback
    m_lp_mpv_render_context_set_update_callback = nullptr;
static lp_mpv_render_context_render m_lp_mpv_render_context_render = nullptr;
static lp_mpv_set_wakeup_callback m_lp_mpv_set_wakeup_callback = nullptr;
static lp_mpv_initialize m_lp_mpv_initialize = nullptr;
static lp_mpv_render_context_free m_lp_mpv_render_context_free = nullptr;
static lp_mpv_terminate_destroy m_lp_mpv_terminate_destroy = nullptr;
static lp_mpv_request_log_messages m_lp_mpv_request_log_messages = nullptr;
static lp_mpv_wait_event m_lp_mpv_wait_event = nullptr;
static lp_mpv_create m_lp_mpv_create = nullptr;
static lp_mpv_event_name m_lp_mpv_event_name = nullptr;
static lp_mpv_free_node_contents m_lp_mpv_free_node_contents = nullptr;
#else
#define m_lp_mpv_get_property mpv_get_property
#define m_lp_mpv_set_property mpv_set_property
#define m_lp_mpv_set_property_async mpv_set_property_async
#define m_lp_mpv_command_node mpv_command_node
#define m_lp_mpv_command_node_async mpv_command_node_async
#define m_lp_mpv_load_config_file mpv_load_config_file
#define m_lp_mpv_error_string mpv_error_string
#define m_lp_mpv_observe_property mpv_observe_property
#define m_lp_mpv_render_context_create mpv_render_context_create
#define m_lp_mpv_render_context_set_update_callback                            \
    mpv_render_context_set_update_callback
#define m_lp_mpv_render_context_render mpv_render_context_render
#define m_lp_mpv_set_wakeup_callback mpv_set_wakeup_callback
#define m_lp_mpv_initialize mpv_initialize
#define m_lp_mpv_render_context_free mpv_render_context_free
#define m_lp_mpv_terminate_destroy mpv_terminate_destroy
#define m_lp_mpv_request_log_messages mpv_request_log_messages
#define m_lp_mpv_wait_event mpv_wait_event
#define m_lp_mpv_create mpv_create
#define m_lp_mpv_event_name mpv_event_name
#define m_lp_mpv_free_node_contents mpv_free_node_contents
#endif

static inline bool libmpv_init(const QString &path) {
#ifdef WWX190_DYNAMIC_LIBMPV
    Q_ASSERT(!path.isEmpty());
    QLibrary library(path);
    // Q_ASSERT(QFileInfo::exists(library.fileName()));
    m_lp_mpv_get_property = reinterpret_cast<lp_mpv_get_property>(
        library.resolve("mpv_get_property"));
    m_lp_mpv_set_property = reinterpret_cast<lp_mpv_set_property>(
        library.resolve("mpv_set_property"));
    m_lp_mpv_set_property_async = reinterpret_cast<lp_mpv_set_property_async>(
        library.resolve("mpv_set_property_async"));
    m_lp_mpv_command_node = reinterpret_cast<lp_mpv_command_node>(
        library.resolve("mpv_command_node"));
    m_lp_mpv_command_node_async = reinterpret_cast<lp_mpv_command_node_async>(
        library.resolve("mpv_command_node_async"));
    m_lp_mpv_load_config_file = reinterpret_cast<lp_mpv_load_config_file>(
        library.resolve("mpv_load_config_file"));
    m_lp_mpv_error_string = reinterpret_cast<lp_mpv_error_string>(
        library.resolve("mpv_error_string"));
    m_lp_mpv_observe_property = reinterpret_cast<lp_mpv_observe_property>(
        library.resolve("mpv_observe_property"));
    m_lp_mpv_render_context_create =
        reinterpret_cast<lp_mpv_render_context_create>(
            library.resolve("mpv_render_context_create"));
    m_lp_mpv_render_context_set_update_callback =
        reinterpret_cast<lp_mpv_render_context_set_update_callback>(
            library.resolve("mpv_render_context_set_update_callback"));
    m_lp_mpv_render_context_render =
        reinterpret_cast<lp_mpv_render_context_render>(
            library.resolve("mpv_render_context_render"));
    m_lp_mpv_set_wakeup_callback = reinterpret_cast<lp_mpv_set_wakeup_callback>(
        library.resolve("mpv_set_wakeup_callback"));
    m_lp_mpv_initialize =
        reinterpret_cast<lp_mpv_initialize>(library.resolve("mpv_initialize"));
    m_lp_mpv_render_context_free = reinterpret_cast<lp_mpv_render_context_free>(
        library.resolve("mpv_render_context_free"));
    m_lp_mpv_terminate_destroy = reinterpret_cast<lp_mpv_terminate_destroy>(
        library.resolve("mpv_terminate_destroy"));
    m_lp_mpv_request_log_messages =
        reinterpret_cast<lp_mpv_request_log_messages>(
            library.resolve("mpv_request_log_messages"));
    m_lp_mpv_wait_event =
        reinterpret_cast<lp_mpv_wait_event>(library.resolve("mpv_wait_event"));
    m_lp_mpv_create =
        reinterpret_cast<lp_mpv_create>(library.resolve("mpv_create"));
    m_lp_mpv_event_name =
        reinterpret_cast<lp_mpv_event_name>(library.resolve("mpv_event_name"));
    m_lp_mpv_free_node_contents = reinterpret_cast<lp_mpv_free_node_contents>(
        library.resolve("mpv_free_node_contents"));
#else
    Q_UNUSED(path)
#endif
    return true;
}

static inline QVariant node_to_variant(const mpv_node *node) {
    switch (node->format) {
    case MPV_FORMAT_STRING:
        return QVariant(QString::fromUtf8(node->u.string));
    case MPV_FORMAT_FLAG:
        return QVariant(static_cast<bool>(node->u.flag));
    case MPV_FORMAT_INT64:
        return QVariant(static_cast<qlonglong>(node->u.int64));
    case MPV_FORMAT_DOUBLE:
        return QVariant(node->u.double_);
    case MPV_FORMAT_NODE_ARRAY: {
        mpv_node_list *list = node->u.list;
        QVariantList qlist;
        for (int n = 0; n < list->num; n++) {
            qlist.append(node_to_variant(&list->values[n]));
        }
        return QVariant(qlist);
    }
    case MPV_FORMAT_NODE_MAP: {
        mpv_node_list *list = node->u.list;
        QVariantMap qmap;
        for (int n = 0; n < list->num; n++) {
            qmap.insert(QString::fromUtf8(list->keys[n]),
                        node_to_variant(&list->values[n]));
        }
        return QVariant(qmap);
    }
    default: // MPV_FORMAT_NONE, unknown values (e.g. future extensions)
        return QVariant();
    }
}

struct node_builder {
    node_builder(const QVariant &v) { set(&node_, v); }
    ~node_builder() { free_node(&node_); }
    mpv_node *node() { return &node_; }

private:
    Q_DISABLE_COPY(node_builder)
    mpv_node node_;
    mpv_node_list *create_list(mpv_node *dst, bool is_map, int num) {
        dst->format = is_map ? MPV_FORMAT_NODE_MAP : MPV_FORMAT_NODE_ARRAY;
        auto *list = new mpv_node_list();
        dst->u.list = list;
        if (list == nullptr) {
            goto err;
        }
        list->values = new mpv_node[num]();
        if (list->values == nullptr) {
            goto err;
        }
        if (is_map) {
            list->keys = new char *[num]();
            if (list->keys == nullptr) {
                goto err;
            }
        }
        return list;
    err:
        free_node(dst);
        return nullptr;
    }
    char *dup_qstring(const QString &s) {
        QByteArray b = s.toUtf8();
        char *r = new char[b.size() + 1];
        if (r != nullptr) {
            std::memcpy(r, b.data(), b.size() + 1);
        }
        return r;
    }
    bool test_type(const QVariant &v, QMetaType::Type t) {
        // The Qt docs say: "Although this function is declared as returning
        // "QVariant::Type(obsolete), the return value should be interpreted
        // as QMetaType::Type."
        // So a cast really seems to be needed to avoid warnings (urgh).
        return static_cast<int>(v.type()) == static_cast<int>(t);
    }
    void set(mpv_node *dst, const QVariant &src) {
        if (test_type(src, QMetaType::QString)) {
            dst->format = MPV_FORMAT_STRING;
            dst->u.string = dup_qstring(src.toString());
            if (dst->u.string == nullptr) {
                goto fail;
            }
        } else if (test_type(src, QMetaType::Bool)) {
            dst->format = MPV_FORMAT_FLAG;
            dst->u.flag = src.toBool() ? 1 : 0;
        } else if (test_type(src, QMetaType::Int) ||
                   test_type(src, QMetaType::LongLong) ||
                   test_type(src, QMetaType::UInt) ||
                   test_type(src, QMetaType::ULongLong)) {
            dst->format = MPV_FORMAT_INT64;
            dst->u.int64 = src.toLongLong();
        } else if (test_type(src, QMetaType::Double)) {
            dst->format = MPV_FORMAT_DOUBLE;
            dst->u.double_ = src.toDouble();
        } else if (src.canConvert<QVariantList>()) {
            QVariantList qlist = src.toList();
            mpv_node_list *list = create_list(dst, false, qlist.size());
            if (list == nullptr) {
                goto fail;
            }
            list->num = qlist.size();
            for (int n = 0; n < qlist.size(); n++) {
                set(&list->values[n], qlist[n]);
            }
        } else if (src.canConvert<QVariantMap>()) {
            QVariantMap qmap = src.toMap();
            mpv_node_list *list = create_list(dst, true, qmap.size());
            if (list == nullptr) {
                goto fail;
            }
            list->num = qmap.size();
            for (int n = 0; n < qmap.size(); n++) {
                list->keys[n] = dup_qstring(qmap.keys()[n]);
                if (list->keys[n] == nullptr) {
                    free_node(dst);
                    goto fail;
                }
                set(&list->values[n], qmap.values()[n]);
            }
        } else {
            goto fail;
        }
        return;
    fail:
        dst->format = MPV_FORMAT_NONE;
    }
    void free_node(mpv_node *dst) {
        switch (dst->format) {
        case MPV_FORMAT_STRING:
            delete[] dst->u.string;
            break;
        case MPV_FORMAT_NODE_ARRAY:
        case MPV_FORMAT_NODE_MAP: {
            mpv_node_list *list = dst->u.list;
            if (list != nullptr) {
                for (int n = 0; n < list->num; n++) {
                    if (list->keys != nullptr) {
                        delete[] list->keys[n];
                    }
                    if (list->values != nullptr) {
                        free_node(&list->values[n]);
                    }
                }
                delete[] list->keys;
                delete[] list->values;
            }
            delete list;
            break;
        }
        default:;
        }
        dst->format = MPV_FORMAT_NONE;
    }
};

/**
 * RAII wrapper that calls mpv_free_node_contents() on the pointer.
 */
struct node_autofree {
    mpv_node *ptr;
    node_autofree(mpv_node *a_ptr) : ptr(a_ptr) {}
    ~node_autofree() { m_lp_mpv_free_node_contents(ptr); }
};

/**
 * This is used to return error codes wrapped in QVariant for functions which
 * return QVariant.
 *
 * You can use get_error() or is_error() to extract the error status from a
 * QVariant value.
 */
struct ErrorReturn {
    /**
     * enum mpv_error value (or a value outside of it if ABI was extended)
     */
    int error = 0;

    explicit ErrorReturn() = default;
    explicit ErrorReturn(int err) : error(err) {}
};

/**
 * Return the mpv error code packed into a QVariant, or 0 (success) if it's not
 * an error value.
 *
 * @return error code (<0) or success (>=0)
 */
static inline int get_error(const QVariant &v) {
    if (!v.canConvert<ErrorReturn>()) {
        return 0;
    }
    return v.value<ErrorReturn>().error;
}

/**
 * Return whether the QVariant carries a mpv error code.
 */
static inline bool is_error(const QVariant &v) { return get_error(v) < 0; }

/**
 * Return the given property as mpv_node converted to QVariant, or QVariant()
 * on error.
 *
 * @param name the property name
 * @return the property value, or an ErrorReturn with the error code
 */
static inline QVariant get_property(mpv_handle *ctx, const QString &name) {
    mpv_node node;
    int err = m_lp_mpv_get_property(ctx, qUtf8Printable(name), MPV_FORMAT_NODE,
                                    &node);
    if (err < 0) {
        return QVariant::fromValue(ErrorReturn(err));
    }
    node_autofree f(&node);
    return node_to_variant(&node);
}

/**
 * Set the given property as mpv_node converted from the QVariant argument.
 *
 * @return mpv error code (<0 on error, >= 0 on success)
 */
static inline int set_property(mpv_handle *ctx, const QString &name,
                               const QVariant &v) {
    node_builder node(v);
    return m_lp_mpv_set_property(ctx, qUtf8Printable(name), MPV_FORMAT_NODE,
                                 node.node());
}

/**
 * Set the given property asynchronously as mpv_node converted from the QVariant
 * argument.
 *
 * @return mpv error code (<0 on error, >= 0 on success)
 */
static inline int set_property_async(mpv_handle *ctx, const QString &name,
                                     const QVariant &v,
                                     quint64 reply_userdata) {
    node_builder node(v);
    return m_lp_mpv_set_property_async(ctx, reply_userdata,
                                       qUtf8Printable(name), MPV_FORMAT_NODE,
                                       node.node());
}

/**
 * mpv_command_node() equivalent.
 *
 * @param args command arguments, with args[0] being the command name as string
 * @return the property value, or an ErrorReturn with the error code
 */
static inline QVariant command(mpv_handle *ctx, const QVariant &args) {
    node_builder node(args);
    mpv_node res;
    int err = m_lp_mpv_command_node(ctx, node.node(), &res);
    if (err < 0) {
        return QVariant::fromValue(ErrorReturn(err));
    }
    node_autofree f(&res);
    return node_to_variant(&res);
}

/**
 * Send commands to mpv asynchronously.
 *
 * @param args command arguments, with args[0] being the command name as string
 * @return mpv error code (<0 on error, >= 0 on success)
 */
static inline int command_async(mpv_handle *ctx, const QVariant &args,
                                quint64 reply_userdata) {
    node_builder node(args);
    return m_lp_mpv_command_node_async(ctx, reply_userdata, node.node());
}

static inline int load_config_file(mpv_handle *ctx, const QString &fileName) {
    return m_lp_mpv_load_config_file(ctx, qUtf8Printable(fileName));
}

static inline int observe_property(mpv_handle *ctx, const QString &name,
                                   quint64 reply_userdata) {
    return m_lp_mpv_observe_property(ctx, reply_userdata, qUtf8Printable(name),
                                     MPV_FORMAT_NONE);
}

static inline QString error_string(int errCode) {
    return QString::fromUtf8(m_lp_mpv_error_string(errCode));
}

static inline int render_context_create(mpv_render_context **res,
                                        mpv_handle *ctx,
                                        mpv_render_param *params) {
    return m_lp_mpv_render_context_create(res, ctx, params);
}

static inline void
render_context_set_update_callback(mpv_render_context *ctx,
                                   mpv_render_update_fn callback,
                                   void *callback_ctx) {
    m_lp_mpv_render_context_set_update_callback(ctx, callback, callback_ctx);
}

static inline int render_context_render(mpv_render_context *ctx,
                                        mpv_render_param *params) {
    return m_lp_mpv_render_context_render(ctx, params);
}

static inline void set_wakeup_callback(mpv_handle *ctx, void (*cb)(void *),
                                       void *d) {
    m_lp_mpv_set_wakeup_callback(ctx, cb, d);
}

static inline int initialize(mpv_handle *ctx) {
    return m_lp_mpv_initialize(ctx);
}

static inline void render_context_free(mpv_render_context *ctx) {
    m_lp_mpv_render_context_free(ctx);
}

static inline void terminate_destroy(mpv_handle *ctx) {
    m_lp_mpv_terminate_destroy(ctx);
}

static inline int request_log_messages(mpv_handle *ctx,
                                       const QString &min_level) {
    return m_lp_mpv_request_log_messages(ctx, qUtf8Printable(min_level));
}

static inline mpv_event *wait_event(mpv_handle *ctx, double timeout) {
    return m_lp_mpv_wait_event(ctx, timeout);
}

static inline mpv_handle *create() { return m_lp_mpv_create(); }

static inline QString event_name(mpv_event_id event) {
    return QString::fromUtf8(m_lp_mpv_event_name(event));
}

} // namespace qt

} // namespace mpv

Q_DECLARE_METATYPE(mpv::qt::ErrorReturn)
