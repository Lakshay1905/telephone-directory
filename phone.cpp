#include <iostream>
#include <fstream>
#include <string>
#include <regex>

class Contact
{
public:
    std::string name;
    std::string phoneNumber;
    std::string email;

    Contact(std::string name, std::string phoneNumber, std::string email)
        : name(name), phoneNumber(phoneNumber), email(email) {}
};

class TreeNode
{
public:
    Contact *contact;
    TreeNode *left;
    TreeNode *right;

    TreeNode(Contact *contact) : contact(contact), left(nullptr), right(nullptr) {}
};

class TelephoneDirectory
{
private:
    TreeNode *root;

    void insert(TreeNode *&node, Contact *contact)
    {
        if (node == nullptr)
        {
            node = new TreeNode(contact);
        }
        else if (contact->name < node->contact->name)
        {
            insert(node->left, contact);
        }
        else
        {
            insert(node->right, contact);
        }
    }

    TreeNode *search(TreeNode *node, const std::string &name)
    {
        if (node == nullptr || node->contact->name == name)
        {
            return node;
        }
        else if (name < node->contact->name)
        {
            return search(node->left, name);
        }
        else
        {
            return search(node->right, name);
        }
    }

    TreeNode *deleteNode(TreeNode *&node, const std::string &name)
    {
        if (node == nullptr)
            return nullptr;

        if (name < node->contact->name)
        {
            node->left = deleteNode(node->left, name);
        }
        else if (name > node->contact->name)
        {
            node->right = deleteNode(node->right, name);
        }
        else
        {
            if (node->left == nullptr)
            {
                TreeNode *temp = node->right;
                delete node->contact;
                delete node;
                return temp;
            }
            else if (node->right == nullptr)
            {
                TreeNode *temp = node->left;
                delete node->contact;
                delete node;
                return temp;
            }

            TreeNode *temp = findMin(node->right);
            node->contact = temp->contact;
            node->right = deleteNode(node->right, temp->contact->name);
        }
        return node;
    }

    TreeNode *findMin(TreeNode *node)
    {
        while (node->left != nullptr)
        {
            node = node->left;
        }
        return node;
    }

    void display(TreeNode *node)
    {
        if (node != nullptr)
        {
            display(node->left);
            std::cout << "Name: " << node->contact->name
                      << ", Phone: " << node->contact->phoneNumber
                      << ", Email: " << node->contact->email << '\n';
            display(node->right);
        }
    }

    bool isValidEmail(const std::string &email)
    {
        const std::regex pattern(R"((\w+)(\.\w+)*@(\w+)(\.\w+)+)");
        return std::regex_match(email, pattern);
    }

    bool isValidPhoneNumber(const std::string &phoneNumber)
    {
        const std::regex pattern(R"(\+?\d{10,15})");
        return std::regex_match(phoneNumber, pattern);
    }

    void writeToFile(TreeNode *node, std::ofstream &outFile)
    {
        if (node != nullptr)
        {
            writeToFile(node->left, outFile);
            outFile << node->contact->name << ","
                    << node->contact->phoneNumber << ","
                    << node->contact->email << '\n';
            writeToFile(node->right, outFile);
        }
    }

    void readFromFile(TreeNode *&node, const std::string &filename)
    {
        std::ifstream inFile(filename);
        if (inFile.is_open())
        {
            std::string line;
            while (std::getline(inFile, line))
            {
                std::string name, phoneNumber, email;
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                if (pos1 != std::string::npos && pos2 != std::string::npos)
                {
                    name = line.substr(0, pos1);
                    phoneNumber = line.substr(pos1 + 1, pos2 - pos1 - 1);
                    email = line.substr(pos2 + 1);
                    addContact(name, phoneNumber, email);
                }
            }
            inFile.close();
        }
    }

public:
    TelephoneDirectory() : root(nullptr) {}

    void addContact(const std::string &name, const std::string &phoneNumber, const std::string &email)
    {
        if (!isValidPhoneNumber(phoneNumber))
        {
            std::cout << "Invalid phone number format.\n";
            return;
        }
        if (!isValidEmail(email))
        {
            std::cout << "Invalid email format.\n";
            return;
        }
        Contact *newContact = new Contact(name, phoneNumber, email);
        insert(root, newContact);
        std::cout << "Contact added successfully.\n";
    }

    void updateContact(const std::string &name, const std::string &phoneNumber, const std::string &email)
    {
        TreeNode *result = search(root, name);
        if (result != nullptr)
        {
            if (isValidPhoneNumber(phoneNumber))
            {
                result->contact->phoneNumber = phoneNumber;
            }
            else
            {
                std::cout << "Invalid phone number format.\n";
                return;
            }
            if (isValidEmail(email))
            {
                result->contact->email = email;
            }
            else
            {
                std::cout << "Invalid email format.\n";
                return;
            }
            std::cout << "Contact updated successfully.\n";
        }
        else
        {
            std::cout << "No contact found with that name.\n";
        }
    }

    void searchContact(const std::string &name)
    {
        TreeNode *result = search(root, name);
        if (result != nullptr)
        {
            std::cout << "Found: Name: " << result->contact->name
                      << ", Phone: " << result->contact->phoneNumber
                      << ", Email: " << result->contact->email << '\n';
        }
        else
        {
            std::cout << "No contact found with that name.\n";
        }
    }

    void deleteContact(const std::string &name)
    {
        if (search(root, name) != nullptr)
        {
            root = deleteNode(root, name);
            std::cout << "Contact deleted successfully.\n";
        }
        else
        {
            std::cout << "No contact found with that name.\n";
        }
    }

    void displayContacts()
    {
        if (root == nullptr)
        {
            std::cout << "The directory is empty.\n";
        }
        else
        {
            std::cout << "Contacts:\n";
            display(root);
        }
    }

    void exportContacts(const std::string &filename)
    {
        std::ofstream outFile(filename);
        if (outFile.is_open())
        {
            writeToFile(root, outFile);
            outFile.close();
            std::cout << "Contacts exported successfully to " << filename << ".\n";
        }
        else
        {
            std::cout << "Error opening file for writing.\n";
        }
    }

    void importContacts(const std::string &filename)
    {
        readFromFile(root, filename);
        std::cout << "Contacts imported successfully from " << filename << ".\n";
    }

    ~TelephoneDirectory()
    {
        while (root != nullptr)
        {
            deleteNode(root, root->contact->name);
        }
    }
};

int main()
{
    TelephoneDirectory directory;
    int choice;

    do
    {
        std::cout << "\nTelephone Directory Menu:\n";
        std::cout << "1. Add Contact\n";
        std::cout << "2. Update Contact\n";
        std::cout << "3. Search Contact\n";
        std::cout << "4. Delete Contact\n";
        std::cout << "5. Display All Contacts\n";
        std::cout << "6. Export Contacts\n";
        std::cout << "7. Import Contacts\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;
        std::cin.ignore(); // Ignore newline character after choice

        std::string name, phoneNumber, email, filename;

        switch (choice)
        {
        case 1:
            std::cout << "Enter name: ";
            std::getline(std::cin, name);
            std::cout << "Enter phone number: ";
            std::getline(std::cin, phoneNumber);
            std::cout << "Enter email: ";
            std::getline(std::cin, email);
            directory.addContact(name, phoneNumber, email);
            break;
        case 2:
            std::cout << "Enter name of the contact to update: ";
            std::getline(std::cin, name);
            std::cout << "Enter new phone number: ";
            std::getline(std::cin, phoneNumber);
            std::cout << "Enter new email: ";
            std::getline(std::cin, email);
            directory.updateContact(name, phoneNumber, email);
            break;
        case 3:
            std::cout << "Enter name to search: ";
            std::getline(std::cin, name);
            directory.searchContact(name);
            break;
        case 4:
            std::cout << "Enter name of the contact to delete: ";
            std::getline(std::cin, name);
            directory.deleteContact(name);
            break;
        case 5:
            directory.displayContacts();
            break;
        case 6:
            std::cout << "Enter filename to export contacts: ";
            std::getline(std::cin, filename);
            directory.exportContacts(filename);
            break;
        case 7:
            std::cout << "Enter filename to import contacts: ";
            std::getline(std::cin, filename);
            directory.importContacts(filename);
            break;
        case 0:
            std::cout << "Exiting the directory.\n";
            break;
        default:
            std::cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}
