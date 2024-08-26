Step Tracker and Group Management System
Overview
This project is a C-based Step Tracker and Group Management System designed to manage individuals and groups participating in a step-tracking competition. It supports functionalities such as adding individuals, creating groups, tracking goals, and generating leaderboards.

Features
Add Individuals: Add new individuals to the system, along with their step counts for a week.
Create Groups: Create groups with specified goals and members.
Check Leaderboard: Generate and display the top 3 individuals based on step counts.
Check Group Achievements: Evaluate whether a group has met its weekly step goal.
Merge Groups: Merge two groups into one, combining goals and members.
Delete Individuals/Groups: Delete individuals from the system and from their associated groups. Groups can also be deleted.
Suggest Goal Updates: Suggest daily goals for individuals based on the top performers.
Display Information: Display details of groups and individuals.
File Structure
input.txt: Stores individual data, including ID, name, age, goal, and step counts.
group.txt: Stores group data, including group ID, name, goal, number of members, and their IDs.
Functions
Core Functions
**Add_Person(ind head, int mem_id, char Name, int age, int ind_goal, int stepcount[])

Adds a new individual to the system.
**Add_group(group ghead, int group_id, char group_name, int group_goal)

Creates a new group and adds it to the system.
**get_top_3(ind *head, int arr, int ptr)

Calculates the top 3 individuals based on their step counts.
*Delete_group(group root, int gr_id)

Deletes a group from the system.
**Delete_Individual(ind root, group groot, int mem_id)

Deletes an individual from the system and removes them from their associated group.
Helper Functions
*Check_individual_rewards(int id, ind head)

Checks and displays rewards for individuals in the top 3.
*Suggest_goal_update(ind head)

Suggests daily goals for individuals based on top performers.
*Check_group_achievement(int group_id, group head)

Checks if a group has met its weekly goal.
*Merge_groups(int id1, int id2, group root)

Merges two groups into one.
*Delete_ind_from_group(group groot, int mem_id)

Deletes an individual from a group.
Main Program
The main program initializes the system by reading individual and group data from input.txt and group.txt. It then enters a menu-driven loop that allows the user to perform various operations such as adding individuals, creating groups, checking achievements, merging groups, and deleting entities.

How to Run
Compile the Code: Use a C compiler to compile the code.

bash
gcc -o step_tracker step_tracker.c
Run the Program: After compilation, run the executable.

bash
./step_tracker
Interact with the Program: Follow the on-screen menu to interact with the program.

Dependencies
C Standard Library: Make sure your environment supports standard C libraries for functions like printf, scanf, malloc, etc.
Future Enhancements
Improve input validation and error handling.
Extend the system to support more complex operations, such as detailed reporting and analytics.
Add a graphical user interface (GUI) for easier interaction.
Contribution
If you wish to contribute to this project, feel free to submit a pull request or open an issue for discussion.

License
This project is licensed by Ketan Tripathi.

This README should provide a clear overview of your project, helping others to understand and use your code effectively.
