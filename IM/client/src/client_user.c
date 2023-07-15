#include "im_client.h"
MYSELF myself;
int init_user_struct(MYSELF *m)
{
    memset(m, 0, sizeof(*m));
    m->w_id = -1;
    m->w_socketfd = -1;
    m->w_cstat = INIT_CSTAT;
    m->w_msgstat = MSG_SNDING;
    m->w_fndcount = 0;

    INIT_LIST_HEAD(&myself.w_flisthd);
    return 0;
}
int user_list_add(FRND_STAT *fs, short cnt)
{
    CLIENT_FRIEND *cf, *cp;
    struct list_head *pos, *head = &myself.w_flisthd;
    for (int i = 0; i < cnt; i++)
    {
        printf("client: user %d, name = %s, id = %d, state = %d\n",
               i, fs[i].f_name, fs[i].f_id, fs[i].f_stat);
    }
    for (int i = 0; i < cnt; i++)
    {
        list_for_each(pos, head)
        {
            cp = (CLIENT_FRIEND *)list_entry(pos, CLIENT_FRIEND, c_node);
            if (cp->c_id == fs[i].f_id)
            {
                strcpy(cp->c_name, fs[i].f_name);
                cp->c_stat = fs[i].f_stat;
                break;
            }
            if (pos != head)
            {
                printf("client: user in the friend list, update status!\n");
                break;
            }

            cf = (CLIENT_FRIEND *)malloc(sizeof(*cf));
            if (cf == NULL)
            {
                printf("client: malloc failed!\n");
                continue;
            }

            printf("client: user NOT in the friend list, add this user now\n");
            memset(cf, 0, sizeof(*cf));
            strncpy(cf->c_name, fs[i].f_name, MAX_NAME_LEN - 1);
            cf->c_id = fs[i].f_id;
            cf->c_stat = fs[i].f_stat;

            list_add(&(cf->c_node), head);
        }
    }
    return 0;
}
int user_list_del(FRND_STAT *fs, short cnt)
{
    CLIENT_FRIEND *cp;
    struct list_head *pos = NULL;
    struct list_head *head = &myself.w_flisthd;

    for (int i = 0; i < cnt; i++)
    {
        list_for_each(pos, head)
        {
            cp = (CLIENT_FRIEND *)list_entry(pos, CLIENT_FRIEND, c_node);
            if (cp->c_id == fs[i].f_id)
            {
                break;
            }
        }
        if (pos == head)
        {
            continue;
        }

        cp = (CLIENT_FRIEND *)list_entry(pos, CLIENT_FRIEND, c_node);
        list_del(&(cp->c_node));
        free(cp);
    }
    return 0;
}
int user_list_update(FRND_STAT *fs, short cnt)
{
    for (int i = 0; i < cnt; i++)
    {
        printf("client: user = %d, name = %s, id = %d, stat = %d\n",
               i, fs[i].f_name, fs[i].f_id, fs[i].f_stat);
    }
    return 0;
}
int user_list_output(FRND_STAT *fs, short cnt)
{
    for (int i = 0; i < cnt; i++)
    {
        printf("client: (all list msg)-> user = %d, name = %s, id = %d, stat = %d\n",
               i, fs[i].f_name, fs[i].f_id, fs[i].f_stat);
    }
    return 0;
}