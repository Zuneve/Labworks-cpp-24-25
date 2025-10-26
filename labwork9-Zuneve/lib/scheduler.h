#pragma once

#include <vector>

#include "MyAny.h"
#include "MyFunction.h"
#include "MyInvoke.h"
#include "MyIsSame.h"


class TTaskScheduler {
public:
    using TaskID = size_t;

    template <typename T>
    struct FutureResult {
        TTaskScheduler *scheduler;
        TaskID id;
    };

    TTaskScheduler() = default;

    ~TTaskScheduler() = default;

    template <typename F, typename... Args>
    TaskID add(F&& f, Args&&... args) {
        static_assert(sizeof...(Args) <= 2, "Max count of arguments: 2");

        TaskID id = tasks_.size();
        tasks_.emplace_back(Task{
            false,
            MakeInvoker(std::forward<F>(f), std::forward<Args>(args)...),
            Any()
        });
        return id;
    }

    template <typename F, typename Obj, typename... Args,
        typename = std::enable_if_t<std::is_member_function_pointer_v<std::decay_t<F>>>>
    TaskID add(F&& f, Obj&& obj, Args&&... args) {
        static_assert(sizeof...(Args) <= 2, "Max count of arguments: 2");

        TaskID id = tasks_.size();
        tasks_.emplace_back(Task{
            false,
            MakeMemberInvoker(std::forward<F>(f),
                              std::forward<Obj>(obj), std::forward<Args>(args)...),
            Any()
        });

        return id;
    }

    template <typename T>
    FutureResult<T> getFutureResult(TaskID id) {
        if (id >= tasks_.size()) {
            throw std::runtime_error("There's no such task in the schedule");
        }
        return FutureResult<T>{this, id};
    }

    template <typename T>
    T getResult(FutureResult<T> fr) {
        if (!tasks_[fr.id].computed) {
            tasks_[fr.id].result = tasks_[fr.id].invoker();
            tasks_[fr.id].computed = true;
        }
        return any_cast<T>(tasks_[fr.id].result);
    }

    template <typename T>
    T getResult(TaskID id) {
        return getResult<T>(getFutureResult<T>(id));
    }

    void executeAll() {
        for (auto& t: tasks_) {
            if (!t.computed) {
                t.result = t.invoker();
                t.computed = true;
            }
        }
    }

private:
    struct Task {
        bool computed;
        Function<Any()> invoker;
        Any result;
    };

    std::vector<Task> tasks_;

    template <typename T>
    static T ExtractArg(FutureResult<T> fr) {
        return fr.scheduler->getResult(fr);
    }

    template <typename T>
    static T &&ExtractArg(T&& v) {
        return std::forward<T>(v);
    }

    template <typename F>
    static Function<Any()> MakeInvoker(F&& f) {
        return Function<Any()>([func = std::forward<F>(f)]() mutable -> Any {
            using R = decltype(MyInvoke(func));
            if constexpr (is_same_v<R, void>) {
                MyInvoke(func);
                return Any();
            } else {
                return Any(MyInvoke(func));
            }
        });
    }

    template <typename F, typename A1>
    static Function<Any()> MakeInvoker(F&& f, A1&& a1) {
        return Function<Any()>(
            [func = std::forward<F>(f), argument1 = std::forward<A1>(a1)]() mutable -> Any {
                auto value1 = ExtractArg(argument1);
                using R = decltype(MyInvoke(func, value1));\
                if constexpr (is_same_v<R, void>) {
                    MyInvoke(func, value1);
                    return Any();
                } else {
                    return Any(MyInvoke(func, value1));
                }
            }
        );
    }


    template <typename F, typename A1, typename A2>
    static Function<Any()> MakeInvoker(F&& f, A1&& a1, A2&& a2) {
        return Function<Any()>(
            [func = std::forward<F>(f),
                argument1 = std::forward<A1>(a1),
                argument2 = std::forward<A2>(a2)]() mutable -> Any {
                auto value1 = ExtractArg(argument1);
                auto value2 = ExtractArg(argument2);
                using R = decltype(MyInvoke(func, value1, value2));
                if constexpr (is_same_v<R, void>) {
                    MyInvoke(func, value1, value2);
                    return Any();
                } else {
                    return Any(MyInvoke(func, value1, value2));
                }
            }
        );
    }


    template <typename F, typename Obj>
    static Function<Any()> MakeMemberInvoker(F&& f, Obj&& obj) {
        return Function<Any()>([func = std::forward<F>(f),
                object = std::forward<Obj>(obj)]() mutable -> Any {
                using R = decltype(MyInvoke(func, object));
                if constexpr (is_same_v<R, void>) {
                    MyInvoke(func, object);
                    return Any();
                } else {
                    return Any(MyInvoke(func, object));
                }
            });
    }

    template <typename F, typename Obj, typename A1>
    static Function<Any()> MakeMemberInvoker(F&& f, Obj&& obj, A1&& a1) {
        return Function<Any()>([func = std::forward<F>(f),
                object = std::forward<Obj>(obj),
                argument1 = std::forward<A1>(a1)]() mutable -> Any {
                auto value1 = ExtractArg(argument1);
                using R = decltype(MyInvoke(func, object, value1));
                if constexpr (is_same_v<R, void>) {
                    MyInvoke(func, object, value1);
                    return Any();
                } else {
                    return Any(MyInvoke(func, object, value1));
                }
            }
        );
    }



    template <typename F, typename Obj, typename A1, typename A2>
    static Function<Any()> MakeMemberInvoker(F&& f, Obj&& obj, A1&& a1, A2&& a2) {
        return Function<Any()>([func = std::forward<F>(f),
                object = std::forward<Obj>(obj),
                argument1 = std::forward<A1>(a1),
                argument2 = std::forward<A2>(a2)]() mutable -> Any {
                auto value1 = ExtractArg(argument1);
                auto value2 = ExtractArg(argument2);
                using R = decltype(MyInvoke(func, object, value1, value2));
                if constexpr (is_same_v<R, void>) {
                    MyInvoke(func, object, value1, value2);
                    return Any();
                } else {
                    return Any(MyInvoke(func, object, value1, value2));
                }
            });
    }
};
