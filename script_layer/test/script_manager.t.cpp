#include <gtest/gtest.h>
#include <map>
#include <memory>
#include "macro_runner.h"
#include "script_manager.h"

class fake_vm : public macro_runner {
public:
    fake_vm(bool canHandleCommand = true) : _handleCommand{canHandleCommand} {}
    bool ticked = false;
    bool activated = false;
    bool handledUnhanledCommand = false;

    void tick() override {
        ticked = true;
    }

    void activate() override {
        activated = true;
    }

    void loadScript(const std::string &string) override {

    }

    bool handleUnhandledCommand(const std::string &command, std::vector<std::string> args) override {
        handledUnhanledCommand = true;
        return _handleCommand;
    }
private:
    bool _handleCommand;
};

TEST(script_manager, will_tick_all_vms) {
    script_manager manager;

    fake_vm *fake_vm1 = new fake_vm;
    fake_vm *fake_vm2 = new fake_vm;

    manager.add_vm("one", fake_vm1);
    manager.add_vm("two", fake_vm2);

    manager.tick();

    EXPECT_TRUE(fake_vm1->ticked);
    EXPECT_TRUE(fake_vm2->ticked);
}

TEST(script_manager, will_activate_all_vms) {
    script_manager manager;

    fake_vm *fake_vm1 = new fake_vm;
    fake_vm *fake_vm2 = new fake_vm;

    manager.add_vm("one", fake_vm1);
    manager.add_vm("two", fake_vm2);

    manager.activate();

    EXPECT_TRUE(fake_vm1->activated);
    EXPECT_TRUE(fake_vm2->activated);
}

TEST(script_manager, will_provide_unhandled_command) {
    script_manager manager;

    fake_vm *fake_vm1 = new fake_vm{false};
    fake_vm *fake_vm2 = new fake_vm{false};

    manager.add_vm("one", fake_vm1);
    manager.add_vm("two", fake_vm2);

    manager.handleUnhandledCommand("command", {"one", "two", "three"});

    EXPECT_TRUE(fake_vm1->handledUnhanledCommand);
    EXPECT_TRUE(fake_vm2->handledUnhanledCommand);
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


TEST(script_manager, only_one_vm_will_handle_a_command) {
    script_manager manager;

    fake_vm *fake_vm1 = new fake_vm{true};
    fake_vm *fake_vm2 = new fake_vm{true};

    manager.add_vm("one", fake_vm1);
    manager.add_vm("two", fake_vm2);

    manager.handleUnhandledCommand("command", {"one", "two", "three"});

    EXPECT_NE(fake_vm1->handledUnhanledCommand, fake_vm2->handledUnhanledCommand);
}