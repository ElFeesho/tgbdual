#include <gtest/gtest.h>
#include <map>
#include <memory>
#include "script_vm.h"
#include "script_manager.h"

class fake_vm : public script_vm {
public:
    fake_vm(bool canHandleCommand = true) : _handleCommand{canHandleCommand} {}
    bool ticked{false};
    bool activated{false};
    bool handledUnhandledCommand{false};

    void tick() override {
        ticked = true;
    }

    void activate() override {
        activated = true;
    }

    void loadScript(const std::string &string) override {

    }

    bool handleUnhandledCommand(const std::string &command, std::vector<std::string> args) override {
        handledUnhandledCommand = true;
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


TEST(script_manager, only_one_vm_will_handle_a_command) {
    script_manager manager;

    fake_vm *fake_vm1 = new fake_vm{true};
    fake_vm *fake_vm2 = new fake_vm{true};

    manager.add_vm("one", fake_vm1);
    manager.add_vm("two", fake_vm2);

    manager.handleUnhandledCommand("command", {"one", "two", "three"});

    EXPECT_NE(fake_vm1->handledUnhandledCommand, fake_vm2->handledUnhandledCommand);
}

TEST(script_manager, removing_non_existant_vm_is_noop) {
    script_manager manager;

    EXPECT_NO_FATAL_FAILURE(manager.remove_vm("doesnt exist"));
}


// Difficult to test these ones... smelly design?
//TEST(script_manager, will_not_tick_unloaded_script_vms) {
//    script_manager manager;
//
//    fake_vm *fake_vm1 = new fake_vm{true};
//    fake_vm *fake_vm2 = new fake_vm{true};
//
//    manager.add_vm("one", fake_vm1);
//    manager.add_vm("two", fake_vm2);
//
//    manager.remove_vm("one");
//
//    manager.tick();
//
//    EXPECT_FALSE(fake_vm1->ticked);
//}
//
//TEST(script_manager, will_not_activate_unloaded_script_vms) {
//    script_manager manager;
//
//    fake_vm *fake_vm1 = new fake_vm{true};
//    bool &activated = fake_vm1->activated;
//    fake_vm *fake_vm2 = new fake_vm{true};
//
//    manager.add_vm("one", fake_vm1);
//    manager.add_vm("two", fake_vm2);
//
//    manager.remove_vm("one");
//
//    manager.activate();
//
//    EXPECT_FALSE(activated);
//}