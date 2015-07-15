#ifndef COMMAND_H_
#define COMMAND_H_

class Command
{
public:
    int get_cmd();
    char* get_arg();
    void set_cmd(int cmd);
    void set_arg(char* arg);
private:
    int cmd_;
    char arg_[65];
};

#endif /* COMMAND_H_ */
