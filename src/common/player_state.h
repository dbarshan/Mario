#pragma once

class PlayerState {
public:
    bool small{true};
    bool big{false};
    bool fire{false};
    bool invincible{false};

    PlayerState() = default;

    [[nodiscard]] bool is_small() const noexcept { return small; }
    [[nodiscard]] bool is_big() const noexcept { return big; }
    [[nodiscard]] bool is_invincible() const noexcept { return invincible; }
    [[nodiscard]] bool can_fire() const noexcept { return fire; }

    void set_small() noexcept { small = true; }
    void set_big() noexcept { big = true; }
    void set_fire() noexcept { fire = true; }
    void set_invincible() noexcept { invincible = true; }

    void unset_invincible() noexcept { invincible = false; }
    void unset_can_fire() noexcept { fire = false; }
    void unset_big() noexcept { big = false; }
    void unset_small() noexcept { small = false; }
};