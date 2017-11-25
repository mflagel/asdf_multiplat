#pragma once

namespace asdf
{
    using interp_func_t = std::function<float(float)>;

    /// sadly constexpr lambdas are not supported yet. supposedly will be supported by C++17?
    // constexpr auto linear_interp_func = [](float _percentage){ return _percentage; };
    #define LINEAR_INTERP_FUNC [](float _percentage) -> float { return _percentage; }

    template <class T>
        // requires Arithmetic<T>
    struct interpolator_
    {
        T start;
        T  dest;
        float current_time{0.0f};
        float   total_time{0.0f};

        interp_func_t interp_func = LINEAR_INTERP_FUNC; // return float between 0 and 1 from dt; I can maybe replace this with a template argument F

        interpolator_() = default;

        //this function needed since I can't use LINEAR_INTERP_FUNC as a default value in the next constructor, as it somehow fails (memory falling out of scope??)
        interpolator_(T _start, T _dest, float _time)
        : start{_start}
        , dest{_dest}
        , total_time{_time}
        {}

        interpolator_(T _start, T _dest, float _time, interp_func_t _func)
        : start{_start}
        , dest{_dest}
        , total_time{_time}
        , interp_func(_func)
        {}

        void reset()
        {
            current_time = 0;
        }
        void reset_smooth() // reset in a smooth way (ie: if max time is 5, current  is 5.1, current should be 0.1 to maintain that amount of time passing, otherwise the whole system loses that 0.1)
        {
            current_time = std::max(0.0f, current_time - total_time);
        }

        bool is_finished() const
        {
            return current_time > total_time;
        }
        float get_percentage() const
        {
            ASSERT(total_time != 0.0f, "why interpolate over an instant time"); // assert to prevent divide by zero
            // LOG_IF(dest - start == T{0}, "why interpolate between identical points"); // potentially unnecessary logging

            float capped_current_time = std::min(current_time, total_time);
            return interp_func(capped_current_time / total_time);
        }

        T calc() const
        {
            return start + (dest - start) * get_percentage();
        }

        void update(float dt)
        {
            current_time += dt;
        }
    };
}