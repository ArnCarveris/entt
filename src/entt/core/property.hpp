#ifndef ENTT_CORE_PROPERTY_HPP
#define ENTT_CORE_PROPERTY_HPP


namespace entt {

    template<typename, typename, char...>
    class Property;

    template<typename Registry>
    class PropertyRegistry {

        template<typename, typename, char...>
        friend class Property;

        template<typename Type>
        static bool setup(const char* name) {
            Registry::registrate<Type>(name);

            return true;
        }
    };

    template <typename Registry, typename Value, char... C>
    class Property {
        static constexpr size_t size = sizeof...(C);
        static constexpr const char buff[size + 1] = { C..., '\0' };

        /*! @brief Name for runtime debug */
        const char* name{ &buff[0] };
        /*! @brief Value */
        Value value{};

        /*! @brief Named property type setup for registration */
        struct auto_setup_t
        {
            auto_setup_t()
            {
                static_assert(std::is_base_of<PropertyRegistry<Registry>, Registry>::value, "!");

                static auto _{ PropertyRegistry<Registry>::setup<Property>(&buff[0]) }; (void)_;
            }
        };
    public:
        using auto_register_type = auto_setup_t;
        using registry_type = Registry;
        using value_type = Value;

        template <typename... Args>
        Property(Args &&... args) : value{ std::forward<Args>(args)... } { auto_setup_t{}; }

        inline operator Value &() { return value; }
        inline operator const Value &() const { return value; }
    };
}
#endif // ENTT_CORE_PROPERTY_HPP
