#pragma once
#include "common/common.hpp"
#include "common/text.hpp"
#include "layer_display.hpp"
#include "selectables.hpp"
#include "selection_filter.hpp"
#include "target.hpp"
#include "triangle.hpp"
#include "fragment_cache.hpp"
#include "util/placement.hpp"
#include "util/text_data.hpp"
#include "color_palette.hpp"
#include <array>
#include <set>
#include <unordered_map>
#include <deque>
#include <list>

namespace horizon {
class Canvas {
    friend Selectables;
    friend class SelectionFilter;
    friend class CanvasAnnotation;

public:
    Canvas();
    virtual ~Canvas()
    {
    }
    virtual void clear();
    void update(const class Symbol &sym, const Placement &transform = Placement(), bool edit = true);
    void update(const class Sheet &sheet);
    void update(const class Padstack &padstack, bool edit = true);
    void update(const class Package &pkg, bool edit = true);
    void update(const class Buffer &buf, class LayerProvider *lp);
    enum class PanelMode { INCLUDE, SKIP };
    void update(const class Board &brd, PanelMode mode = PanelMode::INCLUDE);
    void update(const class Frame &fr, bool edit = true);

    ObjectRef add_line(const std::deque<Coordi> &pts, int64_t width, ColorP color, int layer);
    void remove_obj(const ObjectRef &r);
    void hide_obj(const ObjectRef &r);
    void show_obj(const ObjectRef &r);
    void set_flags(const ObjectRef &r, uint8_t mask_set, uint8_t mask_clear);
    void set_flags_all(uint8_t mask_set, uint8_t mask_clear);

    void show_all_obj();

    virtual void update_markers()
    {
    }

    const LayerDisplay &get_layer_display(int index) const;
    void set_layer_display(int index, const LayerDisplay &ld);
    class SelectionFilter selection_filter;

    void set_layer_color(int layer, const Color &color);

    bool layer_is_visible(int layer) const;

    bool show_all_junctions_in_schematic = false;
    bool show_text_in_tracks = false;
    bool fast_draw = false;

    virtual bool get_flip_view() const
    {
        return false;
    };

    std::pair<Coordf, Coordf> get_bbox(bool visible_only = true) const;

    void set_airwire_filter(class IAirwireFilter *f)
    {
        airwire_filter = f;
    }

    static const int first_overlay_layer = 30000;

protected:
    std::map<int, std::vector<Triangle>> triangles;
    void add_triangle(int layer, const Coordf &p0, const Coordf &p1, const Coordf &p2, ColorP co, uint8_t flg = 0);

    std::map<ObjectRef, std::map<int, std::pair<size_t, size_t>>> object_refs;
    std::vector<ObjectRef> object_refs_current;
    void render(const class Symbol &sym, bool on_sheet = false, bool smashed = false, ColorP co = ColorP::FROM_LAYER);
    void render(const class Junction &junc, bool interactive = true, ObjectType mode = ObjectType::INVALID);
    void render(const class Line &line, bool interactive = true, ColorP co = ColorP::FROM_LAYER);
    void render(const class SymbolPin &pin, bool interactive = true, ColorP co = ColorP::FROM_LAYER);
    void render(const class Arc &arc, bool interactive = true, ColorP co = ColorP::FROM_LAYER);
    void render(const class Sheet &sheet);
    void render(const class SchematicSymbol &sym);
    void render(const class LineNet &line);
    void render(const class NetLabel &label);
    void render(const class BusLabel &label);
    void render(const class Warning &warn);
    void render(const class PowerSymbol &sym);
    void render(const class BusRipper &ripper);
    void render(const class Text &text, bool interactive = true, ColorP co = ColorP::FROM_LAYER);
    void render(const class Padstack &padstack, bool interactive = true);
    void render(const class Polygon &polygon, bool interactive = true, ColorP co = ColorP::FROM_LAYER);
    void render(const class Shape &shape, bool interactive = true);
    void render(const class Hole &hole, bool interactive = true);
    void render(const class Package &package, bool interactive = true, bool smashed = false,
                bool omit_silkscreen = false, bool omit_outline = false);
    void render_pad_overlay(const class Pad &pad);
    void render(const class Pad &pad);
    void render(const class Buffer &buf);
    enum class OutlineMode { INCLUDE, OMIT };
    void render(const class Board &brd, bool interactive = true, PanelMode mode = PanelMode::INCLUDE,
                OutlineMode outline_mode = OutlineMode::INCLUDE);
    void render(const class BoardPackage &pkg, bool interactive = true);
    void render(const class BoardHole &hole, bool interactive = true);
    void render(const class Track &track, bool interactive = true);
    void render(const class Via &via, bool interactive = true);
    void render(const class Dimension &dim);
    void render(const class Frame &frame, bool on_sheet = false);
    void render(const class ConnectionLine &line);
    void render(const class Airwire &airwire);
    void render(const class BoardPanel &panel);

    bool needs_push = true;
    virtual void request_push() = 0;
    virtual void push() = 0;

    void set_lod_size(float size);

    void draw_line(const Coord<float> &a, const Coord<float> &b, ColorP color = ColorP::FROM_LAYER, int layer = 10000,
                   bool tr = true, uint64_t width = 0);
    void draw_cross(const Coord<float> &o, float size, ColorP color = ColorP::FROM_LAYER, int layer = 10000,
                    bool tr = true, uint64_t width = 0);
    void draw_plus(const Coord<float> &o, float size, ColorP color = ColorP::FROM_LAYER, int layer = 10000,
                   bool tr = true, uint64_t width = 0);
    void draw_box(const Coord<float> &o, float size, ColorP color = ColorP::FROM_LAYER, int layer = 10000,
                  bool tr = true, uint64_t width = 0);
    void draw_arc(const Coord<float> &center, float radius, float a0, float a1, ColorP color = ColorP::FROM_LAYER,
                  int layer = 10000, bool tr = true, uint64_t width = 0);
    std::pair<Coordf, Coordf> draw_arc2(const Coord<float> &center, float radius0, float a0, float radius1, float a1,
                                        ColorP color = ColorP::FROM_LAYER, int layer = 10000, bool tr = true,
                                        uint64_t width = 0);
    std::pair<Coordf, Coordf> draw_text0(const Coordf &p, float size, const std::string &rtext, int angle, bool flip,
                                         TextOrigin origin, ColorP color, int layer = 10000, uint64_t width = 0,
                                         bool draw = true, TextData::Font font = TextData::Font::SIMPLEX,
                                         bool center = false, bool mirror = false);

    virtual void draw_bitmap_text(const Coordf &p, float scale, const std::string &rtext, int angle, ColorP color,
                                  int layer)
    {
    }

    virtual std::pair<Coordf, Coordf> measure_bitmap_text(const std::string &text) const
    {
        return {{0, 0}, {0, 0}};
    }

    enum class TextBoxMode { FULL, LOWER, UPPER };

    virtual void draw_bitmap_text_box(const Placement &q, float width, float height, const std::string &s, ColorP color,
                                      int layer, TextBoxMode mode)
    {
    }

    void draw_error(const Coordf &center, float scale, const std::string &text, bool tr = true);
    std::tuple<Coordf, Coordf, Coordi> draw_flag(const Coordf &position, const std::string &txt, int64_t size,
                                                 Orientation orientation, ColorP color = ColorP::FROM_LAYER);
    void draw_lock(const Coordf &center, float size, ColorP color = ColorP::FROM_LAYER, int layer = 10000,
                   bool tr = true);

    virtual void img_net(const class Net *net)
    {
    }
    virtual void img_polygon(const Polygon &poly, bool tr = true)
    {
    }
    virtual void img_padstack(const Padstack &ps)
    {
    }
    virtual void img_set_padstack(bool v)
    {
    }
    virtual void img_line(const Coordi &p0, const Coordi &p1, const uint64_t width, int layer = 10000, bool tr = true);
    virtual void img_hole(const Hole &hole)
    {
    }
    virtual void img_patch_type(PatchType type)
    {
    }
    virtual void img_text(const Text &txt, std::pair<Coordf, Coordf> &extents)
    {
    }
    virtual void img_draw_text(const Coordf &p, float size, const std::string &rtext, int angle, bool flip,
                               TextOrigin origin, int layer = 10000, uint64_t width = 0,
                               TextData::Font font = TextData::Font::SIMPLEX, bool center = false, bool mirror = false)
    {
    }
    bool img_mode = false;
    bool img_auto_line = false;

    Placement transform;
    void transform_save();
    void transform_restore();
    std::list<Placement> transforms;

    Selectables selectables;
    std::vector<Target> targets;
    Target target_current;

    const class LayerProvider *layer_provider = nullptr;
    std::map<int, Color> layer_colors;
    Color get_layer_color(int layer) const;
    int work_layer = 0;
    std::map<int, LayerDisplay> layer_display;

    UUID sheet_current_uuid;

    Triangle::Type triangle_type_current = Triangle::Type::NONE;

    std::map<std::pair<int, bool>, int> overlay_layers;
    int overlay_layer_current = first_overlay_layer;
    int get_overlay_layer(int layer, bool ignore_flip = false);

    FragmentCache fragment_cache;

private:
    uint8_t lod_current = 0;
    class IAirwireFilter *airwire_filter = nullptr;
};
} // namespace horizon
