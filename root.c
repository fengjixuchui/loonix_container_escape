#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/fs_struct.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>

#define PROCFS_NAME "root"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("null333");
MODULE_DESCRIPTION("intentionally vulnerable kernel module");
MODULE_VERSION("1.0");


int procfs_open(struct inode *inode, struct file *file);
static int procfs_show(struct seq_file *m, void *v);


struct proc_dir_entry *procfile_entry;

static const struct proc_ops procfile_fops = {
    .proc_open = procfs_open,
    .proc_read = seq_read
};


static int __init mod_init(void)
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

static void __exit mod_exit(void)
{
    remove_proc_entry(PROCFS_NAME, NULL);
    printk(KERN_INFO "procfile removed\n");
}

int procfs_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "escalating pid %i to root\n", current->pid);

    struct task_struct *init_proc = pid_task(find_vpid(1), PIDTYPE_PID);
    struct cred *new_cred = prepare_creds();
    new_cred = init_proc->cred;
    commit_creds(new_cred);

	return 0;
}

static int procfs_show(struct seq_file *m, void *v)
{
    return 0;
}

module_init(mod_init);
module_exit(mod_exit);
