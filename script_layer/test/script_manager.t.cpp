#include <gtest/gtest.h>
#include <map>
#include <memory>
#include "script_vm.h"
#include "script_manager.h"

class fake_vm : public script_vm {
public:
    fake_vm(bool canHandleCommand = true, std::function<void()> activateDelegate = []{}) : _handleCommand{canHandleCommand}, _activateDelegate{activateDelegate} {}
    bool ticked{false};
    bool activated{false};
    bool handledUnhandledCommand{false};

    void tick() override {
        ticked = true;
    }

    void activate() override {
        activated = true;
        _activateDelegate();
    }

    void loadScript(const std::string &string) override {

    }

    bool handleUnhandledCommand(const std::string &command, std::vector<std::string> args) override {
        handledUnhandledCommand = true;
        return _handleCommand;
    }
private:
    bool _handleCommand;
    std::function<void()> _activateDelegate;
};


class ScriptManagerTest : public ::testing::Test {
public:
    script_manager manager;
    fake_vm *fake_vm1 = new fake_vm;
    fake_vm *fake_vm2 = new fake_vm;
};

TEST_F(ScriptManagerTest, will_tick_all_vms) {
    manager.add_vm("one", fake_vm1);
    manager.add_vm("two", fake_vm2);

    manager.tick();

    EXPECT_TRUE(fake_vm1->ticked);
    EXPECT_TRUE(fake_vm2->ticked);
}

TEST_F(ScriptManagerTest, will_activate_all_vms) {
    manager.add_vm("one", fake_vm1);
    manager.add_vm("two", fake_vm2);

    manager.activate();

    EXPECT_TRUE(fake_vm1->activated);
    EXPECT_TRUE(fake_vm2->activated);
}

TEST_F(ScriptManagerTest, will_provide_unhandled_command) {
    fake_vm *fake_vm1 = new fake_vm{false};
    fake_vm *fake_vm2 = new fake_vm{false};

    manager.add_vm("one", fake_vm1);
    manager.add_vm("two", fake_vm2);

    manager.handleUnhandledCommand("command", {"one", "two", "three"});

    EXPECT_TRUE(fake_vm1->handledUnhandledCommand);
    EXPECT_TRUE(fake_vm2->handledUnhandledCommand);
}


TEST(script_manager, will_report_false_when_no_vms_handle_unhandled_command) {
    script_manager manager;

    fake_vm *fake_vm1 = new fake_vm{false};
    fake_vm *fake_vm2 = new fake_vm{false};

    manager.add_vm("one", fake_vm1);
    manager.add_vm("two", fake_vm2);

    EXPECT_FALSE(manager.handleUnhandledCommand("command", {"one", "two", "three"}));
}

TEST(script_manager, will_report_true_when_a_vms_handle_unhandled_command) {
    script_manager manager;

    fake_vm *fake_vm1 = new fake_vm{false};
    fake_vm *fake_vm2 = new fake_vm{true};

    manager.add_vm("one", fake_vm1);
    manager.add_vm("two", fake_vm2);

    EXPECT_TRUE(manager.handleUnhandledCommand("command", {"one", "two", "three"}));
}


TEST_F(ScriptManagerTest, only_one_vm_will_handle_a_command) {
    manager.add_vm("one", fake_vm1);
    manager.add_vm("two", fake_vm2);

    manager.handleUnhandledCommand("command", {"one", "two", "three"});

    EXPECT_NE(fake_vm1->handledUnhandledCommand, fake_vm2->handledUnhandledCommand);
}

TEST_F(ScriptManagerTest, removing_non_existant_vm_is_noop) {
    EXPECT_NO_FATAL_FAILURE(manager.remove_vm("doesnt exist"));
}


TEST_F(ScriptManagerTest, will_not_tick_unloaded_script_vms) {
    manager.add_vm("one", fake_vm1);
    manager.add_vm("two", fake_vm2);

    manager.remove_vm("one");

    manager.tick();

    EXPECT_FALSE(fake_vm1->ticked);
}

TEST_F(ScriptManagerTest, will_not_activate_unloaded_script_vms) {
    bool &activated = fake_vm1->activated;

    manager.add_vm("one", fake_vm1);
    manager.add_vm("two", fake_vm2);

    manager.remove_vm("one");

    manager.activate();

    EXPECT_FALSE(activated);
}