#include <iostream>
#include <string>
#include <sstream>
#include "Contact.h"
#include "ResponseManager.h"

using namespace std;

int testsPassed = 0;
int testsFailed = 0;

void assertTest(bool condition, const string& testName) {
    if (condition) {
        cout << "[PASS] " << testName << endl;
        testsPassed++;
    } else {
        cout << "[FAIL] " << testName << endl;
        testsFailed++;
    }
}

int main() {
    cout << "====================================================" << endl;
    cout << "       RUNNING UNIT TESTS FOR CONTACT PRIORITY      " << endl;
    cout << "====================================================\n" << endl;

    // ---------------------------------------------------------
    // TEST SUITE 1: Default Constructor & Backward Compatibility
    // ---------------------------------------------------------
    cout << "--- Suite 1: Default Constructor & Backward Compatibility ---" << endl;
    {
        // Default constructor
        Contact defaultContact;
        assertTest(defaultContact.getContactID() == 0, "Default contact ID is 0");
        assertTest(defaultContact.getName() == "Unknown", "Default contact name is 'Unknown'");
        assertTest(defaultContact.getPhoneNumber() == "N/A", "Default phone is 'N/A'");
        assertTest(defaultContact.getRelationship() == "General", "Default relationship is 'General'");
        assertTest(defaultContact.getPriorityLevel() == 1, "Default priority is 1");
        assertTest(defaultContact.getPriorityDescription() == "Primary Contact", "Default priority description is 'Primary Contact'");

        // Old 4-parameter constructor call (backward compatibility)
        Contact oldContact(101, "Alice Smith", "555-0101", "Mother");
        assertTest(oldContact.getContactID() == 101, "4-parameter contact ID preserved");
        assertTest(oldContact.getName() == "Alice Smith", "4-parameter contact name preserved");
        assertTest(oldContact.getPhoneNumber() == "555-0101", "4-parameter phone preserved");
        assertTest(oldContact.getRelationship() == "Mother", "4-parameter relationship preserved");
        assertTest(oldContact.getPriorityLevel() == 1, "4-parameter call defaults to Priority 1");
        assertTest(oldContact.getPriorityDescription() == "Primary Contact", "4-parameter description is 'Primary Contact'");
    }

    // ---------------------------------------------------------
    // TEST SUITE 2: Explicit Priorities 1, 2, and 3
    // ---------------------------------------------------------
    cout << "\n--- Suite 2: Explicit Priorities 1, 2, and 3 ---" << endl;
    {
        // Priority 1: Primary Contact
        Contact c1(201, "Bob Jones", "555-0201", "Spouse", 1);
        assertTest(c1.getPriorityLevel() == 1, "Priority 1 correctly stored");
        assertTest(c1.getPriorityDescription() == "Primary Contact", "Priority 1 description is 'Primary Contact'");

        // Priority 2: Secondary Contact
        Contact c2(202, "Charlie Brown", "555-0202", "Friend", 2);
        assertTest(c2.getPriorityLevel() == 2, "Priority 2 correctly stored");
        assertTest(c2.getPriorityDescription() == "Secondary Contact", "Priority 2 description is 'Secondary Contact'");

        // Priority 3: Emergency / Campus Security Contact
        Contact c3(203, "Campus Security", "555-0911", "Security Dispatch", 3);
        assertTest(c3.getPriorityLevel() == 3, "Priority 3 correctly stored");
        assertTest(c3.getPriorityDescription() == "Emergency / Campus Security Contact", "Priority 3 description is 'Emergency / Campus Security Contact'");
    }

    // ---------------------------------------------------------
    // TEST SUITE 3: Constructor Invalid Priority Validation
    // ---------------------------------------------------------
    cout << "\n--- Suite 3: Constructor Invalid Priority Validation ---" << endl;
    {
        // Priority < 1
        Contact cZero(301, "Dave", "555-0301", "Brother", 0);
        assertTest(cZero.getPriorityLevel() == 1, "Priority 0 defaults to Priority 1");
        assertTest(cZero.getPriorityDescription() == "Primary Contact", "Priority 0 description defaults to 'Primary Contact'");

        Contact cNegative(302, "Eve", "555-0302", "Sister", -5);
        assertTest(cNegative.getPriorityLevel() == 1, "Negative priority (-5) defaults to Priority 1");

        // Priority > 3
        Contact cFour(303, "Frank", "555-0303", "Colleague", 4);
        assertTest(cFour.getPriorityLevel() == 1, "Priority 4 defaults to Priority 1");

        Contact cLarge(304, "Grace", "555-0304", "Neighbor", 99);
        assertTest(cLarge.getPriorityLevel() == 1, "Priority 99 defaults to Priority 1");
    }

    // ---------------------------------------------------------
    // TEST SUITE 4: setPriorityLevel() Validation
    // ---------------------------------------------------------
    cout << "\n--- Suite 4: setPriorityLevel() Validation ---" << endl;
    {
        Contact contact(401, "Hank", "555-0401", "Roommate", 1);
        assertTest(contact.getPriorityLevel() == 1, "Initial priority is 1");

        // Valid updates
        assertTest(contact.setPriorityLevel(2), "setPriorityLevel(2) returns true");
        assertTest(contact.getPriorityLevel() == 2, "Priority successfully updated to 2");
        assertTest(contact.getPriorityDescription() == "Secondary Contact", "Description updated to 'Secondary Contact'");

        assertTest(contact.setPriorityLevel(3), "setPriorityLevel(3) returns true");
        assertTest(contact.getPriorityLevel() == 3, "Priority successfully updated to 3");
        assertTest(contact.getPriorityDescription() == "Emergency / Campus Security Contact", "Description updated to 'Emergency / Campus Security Contact'");

        assertTest(contact.setPriorityLevel(1), "setPriorityLevel(1) returns true");
        assertTest(contact.getPriorityLevel() == 1, "Priority successfully updated back to 1");

        // Invalid updates (must return false and leave existing priority unchanged)
        assertTest(!contact.setPriorityLevel(0), "setPriorityLevel(0) returns false");
        assertTest(contact.getPriorityLevel() == 1, "Priority remains 1 after rejected 0");

        assertTest(!contact.setPriorityLevel(4), "setPriorityLevel(4) returns false");
        assertTest(contact.getPriorityLevel() == 1, "Priority remains 1 after rejected 4");

        assertTest(!contact.setPriorityLevel(-2), "setPriorityLevel(-2) returns false");
        assertTest(contact.getPriorityLevel() == 1, "Priority remains 1 after rejected -2");
    }

    // ---------------------------------------------------------
    // TEST SUITE 5: Field Preservation & Display Verification
    // ---------------------------------------------------------
    cout << "\n--- Suite 5: Field Preservation & Display Output ---" << endl;
    {
        Contact contact(501, "Dr. Banner", "555-0501", "Physician", 2);
        assertTest(contact.getContactID() == 501, "ID 501 preserved");
        assertTest(contact.getName() == "Dr. Banner", "Name 'Dr. Banner' preserved");
        assertTest(contact.getPhoneNumber() == "555-0501", "Phone '555-0501' preserved");
        assertTest(contact.getRelationship() == "Physician", "Relationship 'Physician' preserved");
        assertTest(contact.getPriorityLevel() == 2, "Priority 2 preserved");

        cout << "\nVisual display output:" << endl;
        contact.displayContact();
    }

    // ---------------------------------------------------------
    // TEST SUITE 6: ResponseManager Registration & Display
    // ---------------------------------------------------------
    cout << "\n--- Suite 6: ResponseManager Integration ---" << endl;
    {
        ResponseManager rm;
        rm.registerContact(Contact(601, "Parent 1", "555-0601", "Father", 1));
        rm.registerContact(Contact(602, "Friend 1", "555-0602", "Best Friend", 2));
        rm.registerContact(Contact(603, "Police/Security", "911", "Emergency Services", 3));

        assertTest(rm.getContactCount() == 3, "All 3 contacts registered in ResponseManager");

        cout << "\nResponseManager Display Contacts output:" << endl;
        rm.displayContacts();
    }

    // ---------------------------------------------------------
    // SUMMARY
    // ---------------------------------------------------------
    cout << "\n====================================================" << endl;
    cout << "CONTACT TEST SUMMARY: " << testsPassed << " PASSED, " << testsFailed << " FAILED" << endl;
    cout << "====================================================" << endl;

    return (testsFailed == 0) ? 0 : 1;
}

