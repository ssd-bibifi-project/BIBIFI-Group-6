#include <sstream>
#include <iostream>
#include <vector>
#include <assert.h>
#include <fstream>

#include <efs/efs.h>
#include <efs/cli.h>
#include <efs/database.h>

Efs::Efs::Efs(int argc, char** argv) {
  Database database;

  // initial check if admin user is there. If not then create the admin user
  if (!database.doesUserExist("admin")) {
    database.createUser("admin");
    std::cout << "Created admin! Since this is first run, exiting!" << std::endl;
    return;
  }

  // if keyfile not inputted, ask for keyfile
  if (argv[1] == NULL) {
    std::cout << "Please enter keyfile!" << std::endl;
    std::cout << "Example: ./fileserver <keyfile_name>" << std::endl;
    return;
  }

  // get the keyfile name
  std::string keyfile_name = argv[1];

  /* KEEP TRACK OF VARIABLES HERE */
  // create a new CLI object
  CLI cli;
  std::string currentDir = "/";
  std::string currentUser;

  // Open and read the key file
  std::ifstream infile(keyfile_name);
  if (!infile) {
    std::cerr << "Error: Could not open key file '" << keyfile_name << "'" << std::endl;
    return;
  }
  std::string privateKeyStr((std::istreambuf_iterator<char>(infile)),
                            std::istreambuf_iterator<char>());
  if (infile.fail() && !infile.eof()) {
    std::cerr << "Error: Failed to read data from file '" << keyfile_name << "'" << std::endl;
    infile.close();
    return;
  }
  infile.close();
  
  // Validate keyfile to get who the current user is
  currentUser = database.getUsernameByPrivateKey(privateKeyStr);
  if (currentDir == "") {
    std::cout << "No record found!" << std::endl;
    return;
  } else {
    std::cout << "Welcome back, " + currentUser + "!" << std::endl;
  }

  // TODO: Login as the current user and update the current dir

  // main loop
  while (true) {
    std::string cmd;
    std::cout << "(EFS)$ ";
    std::getline(std::cin, cmd);

    // get command into a vector
    std::vector<std::string> v_cmd;
    std::string tmp;
    std::stringstream ss(cmd);

    // if cmd is empty, just loop again
    if (cmd.empty()) {
      continue;
    }

    while(getline(ss, tmp, ' ')) {
      v_cmd.push_back(tmp);
    }
  
    // Check commands
    if (v_cmd[0] == "cd") {
      if (v_cmd.size() != 2) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: cd <directory>\n";
        continue;
      }

      // declare and initialize targetDir
      std::string targetDir;

      // get target directory from command line arguments
      if (v_cmd[1][0] == '/') {
        targetDir = v_cmd[1];
      } else {
        targetDir = currentDir + "/" + v_cmd[1];
      }

      // check whether target directory exists
      if (!std::filesystem::exists(targetDir)) {
        std::cout << "Directory does not exist\n";
        continue;
      }

      // normalize target directory path
      targetDir = std::filesystem::path(targetDir).lexically_normal().string();
      currentDir = targetDir;
    } else if (v_cmd[0] == "pwd") {
      if (v_cmd.size() != 1) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: pwd\n";
        continue;
      }
      cli.pwd(currentDir);
    } else if (v_cmd[0] == "ls") {
      if (v_cmd.size() != 1) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: ls\n";
        continue;
      }
      cli.ls(currentDir);
    } else if (v_cmd[0] == "cat") {
      if (v_cmd.size() != 2) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: cat <filename>\n";
        continue;
      }
      cli.cat(currentDir, v_cmd[1]);
    } else if (v_cmd[0] == "share") {
      if (v_cmd.size() != 3) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: share <filename> <username>\n";
        continue;
      }
      cli.share(currentDir, v_cmd[1], v_cmd[2]);
    } else if (v_cmd[0] == "mkdir") {
      if (v_cmd.size() != 2) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: mkdir <directory>\n";
        continue;
      }
      cli.mkdir(currentDir, v_cmd[1]);
    } else if (v_cmd[0] == "mkfile") {
      if (v_cmd.size() != 3) {
        std::cout << "Incorrect number of arguments!\n";
        std::cout << "Usage: mkfile <filename> <contents>\n";
        continue;
      }
      cli.mkfile(currentDir, v_cmd[1], v_cmd[2]);
    } else if (v_cmd[0] == "exit") {
      std::cout << "Exiting" << std::endl;
      return;
    } else {
      std::cout << "Please refer to user manual for valid commands\n";
    }
  }
}
