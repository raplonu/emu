#pragma once

#include <pybind11/stl_bind.h>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <type_traits>

namespace emu::bind::map
{

namespace detail
{

    template <typename Map, typename Class_>
    void map_assignment(Class_ &cl) {
        using KeyType = typename Map::key_type;
        using MappedType = typename Map::mapped_type;

        if constexpr (std::is_copy_assignable_v<typename Map::mapped_type>) {

            // Map assignment when copy-assignable: just copy the value
            cl.def("__setitem__", [](Map &m, const KeyType &k, const MappedType &v) {
                auto it = m.find(k);
                if (it != m.end()) {
                    it->second = v;
                } else {
                    m.emplace(k, v);
                }
            });

        } else if constexpr (std::is_copy_constructible_v<typename Map::mapped_type>) {

            // Not copy-assignable, but still copy-constructible: we can update the value by erasing and
            // reinserting
            cl.def("__setitem__", [](Map &m, const KeyType &k, const MappedType &v) {
                // We can't use m[k] = v; because value type might not be default constructable
                auto r = m.emplace(k, v);
                if (!r.second) {
                    // value type is not copy assignable so the only way to insert it is to erase it
                    // first...
                    m.erase(r.first);
                    m.emplace(k, v);
                }
            });
        }

    }

    template <typename Map, typename Class_>
    void repr(Class_ &cl, const std::string& name) {
        if constexpr (emu::cpts::formattable<Map>)
            cl.def("__repr__",
                [name](Map &m) { return fmt::format("{}{{{}}}", name, m); },
                "Return the canonical string representation of this map.");
    }

    template <typename KeyType>
    struct keys_view {
        virtual size_t len() = 0;
        virtual py::iterator iter() = 0;
        virtual bool contains(const KeyType &k) = 0;
        virtual bool contains(const py::object &k) = 0;
        virtual ~keys_view() = default;
    };

    template <typename MappedType>
    struct values_view {
        virtual size_t len() = 0;
        virtual py::iterator iter() = 0;
        virtual ~values_view() = default;
    };

    template <typename KeyType, typename MappedType>
    struct items_view {
        virtual size_t len() = 0;
        virtual py::iterator iter() = 0;
        virtual ~items_view() = default;
    };

    template <typename Map, typename KeysView>
    struct KeysViewImpl : public KeysView {
        explicit KeysViewImpl(Map &map) : map(map) {}
        size_t len() override { return map.size(); }
        py::iterator iter() override { return py::make_key_iterator(map.begin(), map.end()); }
        bool contains(const typename Map::key_type &k) override { return map.find(k) != map.end(); }
        bool contains(const py::object &) override { return false; }
        Map &map;
    };

    template <typename Map, typename ValuesView>
    struct ValuesViewImpl : public ValuesView {
        explicit ValuesViewImpl(Map &map) : map(map) {}
        size_t len() override { return map.size(); }
        py::iterator iter() override { return py::make_value_iterator(map.begin(), map.end()); }
        Map &map;
    };

    template <typename Map, typename ItemsView>
    struct ItemsViewImpl : public ItemsView {
        explicit ItemsViewImpl(Map &map) : map(map) {}
        size_t len() override { return map.size(); }
        py::iterator iter() override { return py::make_iterator(map.begin(), map.end()); }
        Map &map;
    };


} // namespace detail



    template <typename Map, typename Class_>
    auto lookup(Class_& cl) {

        using KeyType = typename Map::key_type;
        using MappedType = typename Map::mapped_type;
        using StrippedKeyType = std::remove_cvref_t<KeyType>;
        using StrippedMappedType = std::remove_cvref_t<MappedType>;
        using KeysView = detail::keys_view<StrippedKeyType>;
        using ValuesView = detail::values_view<StrippedMappedType>;
        using ItemsView = detail::items_view<StrippedKeyType, StrippedMappedType>;

        namespace py = pybind11;

        using KeyType = typename Map::key_type;

        cl.def(
            "__bool__",
            [](const Map &m) -> bool { return !m.empty(); },
            "Check whether the map is nonempty");

        cl.def(
            "__iter__",
            [](Map &m) { return py::make_key_iterator(m.begin(), m.end()); },
            py::keep_alive<0, 1>() /* Essential: keep map alive while iterator exists */
        );

        cl.def(
            "keys",
            [](Map &m) {
                return std::unique_ptr<KeysView>(new detail::KeysViewImpl<Map, KeysView>(m));
            },
            py::keep_alive<0, 1>() /* Essential: keep map alive while view exists */
        );

        cl.def(
            "values",
            [](Map &m) {
                return std::unique_ptr<ValuesView>(new detail::ValuesViewImpl<Map, ValuesView>(m));
            },
            py::keep_alive<0, 1>() /* Essential: keep map alive while view exists */
        );

        cl.def(
            "items",
            [](Map &m) {
                return std::unique_ptr<ItemsView>(new detail::ItemsViewImpl<Map, ItemsView>(m));
            },
            py::keep_alive<0, 1>() /* Essential: keep map alive while view exists */
        );

        cl.def("__contains__", [](Map &m, const KeyType &k) -> bool {
            auto it = m.find(k);
            if (it == m.end()) {
                return false;
            }
            return true;
        });
        // Fallback for when the object is not of the key type
        cl.def("__contains__", [](Map &, const py::object &) -> bool { return false; });

        cl.def("__len__", &Map::size);

        cl.def("__getitem__",
            [](Map &m, const KeyType &k) -> MappedType & {
                auto it = m.find(k);
                if (it == m.end()) {
                    throw py::key_error();
                }
                return it->second;
            },
            py::return_value_policy::reference_internal // ref + keepalive
        );

        return cl;
    }

    template <typename Map, typename Class_>
    auto modifiers(Class_& cl) {

        using KeyType = typename Map::key_type;
        using MappedType = typename Map::mapped_type;
        using StrippedKeyType = std::remove_cvref_t<KeyType>;
        using StrippedMappedType = std::remove_cvref_t<MappedType>;
        using KeysView = detail::keys_view<StrippedKeyType>;
        using ValuesView = detail::values_view<StrippedMappedType>;
        using ItemsView = detail::items_view<StrippedKeyType, StrippedMappedType>;


        // Assignment provided only if the type is copyable
        detail::map_assignment<Map, Class_>(cl);

        cl.def("__delitem__", [](Map &m, const KeyType &k) {
            auto it = m.find(k);
            if (it == m.end()) {
                throw py::key_error();
            }
            m.erase(it);
        });

    }


    template <typename Map, typename holder_type = std::unique_ptr<Map>, typename... Args>
    py::class_<Map, holder_type> init(py::handle scope, const std::string &name, Args &&...args) {
        using KeyType = typename Map::key_type;
        using MappedType = typename Map::mapped_type;
        using StrippedKeyType = std::remove_cvref_t<KeyType>;
        using StrippedMappedType = std::remove_cvref_t<MappedType>;
        using KeysView = detail::keys_view<StrippedKeyType>;
        using ValuesView = detail::values_view<StrippedMappedType>;
        using ItemsView = detail::items_view<StrippedKeyType, StrippedMappedType>;
        using Class_ = py::class_<Map, holder_type>;

        // If either type is a non-module-local bound type then make the map binding non-local as well;
        // otherwise (e.g. both types are either module-local or converting) the map will be
        // module-local.
        auto *tinfo = py::detail::get_type_info(typeid(MappedType));
        bool local = !tinfo || tinfo->module_local;
        if (local) {
            tinfo = py::detail::get_type_info(typeid(KeyType));
            local = !tinfo || tinfo->module_local;
        }

        Class_ cl(scope, name.c_str(), pybind11::module_local(local), std::forward<Args>(args)...);
        static constexpr auto key_type_descr = py::detail::make_caster<KeyType>::name;
        static constexpr auto mapped_type_descr = py::detail::make_caster<MappedType>::name;
        std::string key_type_name(key_type_descr.text), mapped_type_name(mapped_type_descr.text);

        // If key type isn't properly wrapped, fall back to C++ names
        if (key_type_name == "%") {
            key_type_name = py::detail::type_info_description(typeid(KeyType));
        }
        // Similarly for value type:
        if (mapped_type_name == "%") {
            mapped_type_name = py::detail::type_info_description(typeid(MappedType));
        }

        // Wrap KeysView[KeyType] if it wasn't already wrapped
        if (!py::detail::get_type_info(typeid(KeysView))) {
            py::class_<KeysView> keys_view(
                scope, ("KeysView[" + key_type_name + "]").c_str(), pybind11::module_local(local));
            keys_view.def("__len__", &KeysView::len);
            keys_view.def("__iter__",
                        &KeysView::iter,
                        py::keep_alive<0, 1>() /* Essential: keep view alive while iterator exists */
            );
            keys_view.def("__contains__",
                        static_cast<bool (KeysView::*)(const KeyType &)>(&KeysView::contains));
            // Fallback for when the object is not of the key type
            keys_view.def("__contains__",
                        static_cast<bool (KeysView::*)(const py::object &)>(&KeysView::contains));
        }
        // Similarly for ValuesView:
        if (!py::detail::get_type_info(typeid(ValuesView))) {
            py::class_<ValuesView> values_view(scope,
                                        ("ValuesView[" + mapped_type_name + "]").c_str(),
                                        pybind11::module_local(local));
            values_view.def("__len__", &ValuesView::len);
            values_view.def("__iter__",
                            &ValuesView::iter,
                            py::keep_alive<0, 1>() /* Essential: keep view alive while iterator exists */
            );
        }
        // Similarly for ItemsView:
        if (!py::detail::get_type_info(typeid(ItemsView))) {
            py::class_<ItemsView> items_view(
                scope,
                ("ItemsView[" + key_type_name + ", ").append(mapped_type_name + "]").c_str(),
                pybind11::module_local(local));
            items_view.def("__len__", &ItemsView::len);
            items_view.def("__iter__",
                        &ItemsView::iter,
                        py::keep_alive<0, 1>() /* Essential: keep view alive while iterator exists */
            );
        }

        // cl.def(init<>());

        // Register repr (if possible)
        detail::repr<Map>(cl, name);

        return cl;
    }


} // namespace emu::bind::map
