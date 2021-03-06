/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ylegzoul <ylegzoul@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/06/09 15:45:33 by ylegzoul          #+#    #+#             */
/*   Updated: 2020/06/09 15:45:35 by ylegzoul         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/philosophers.h"

t_data			*g_data;

int				main(int ac, char **av)
{
	t_data	data;

	if (ac > 6 || ac < 5)
		return (0);
	g_data = &data;
	if (!(init_game(ac, av)))
		return (0);
	if (!(start_game()))
		return (0);
	sem_wait(g_data->end);
	sem_post(g_data->end);
	clean();
	return (0);
}

int				init_game(int ac, char **av)
{
	g_data->nb = ft_atoi(av[1]);
	if (g_data->nb <= 1 || g_data->nb > 1500)
	{
		write(1, "nb of philo not aviable\n", 24);
		return (0);
	}
	g_data->time_to_die = ft_atoi(av[2]);
	g_data->time_to_eat = ft_atoi(av[3]);
	g_data->time_to_sleep = ft_atoi(av[4]);
	if (g_data->time_to_die < 0 ||
		g_data->time_to_eat < 0 || g_data->time_to_sleep < 0)
	{
		write(1, "bad arguments\n", 14);
		return (0);
	}
	if (ac == 6)
		g_data->number_of_time_each_philosophers_must_eat = ft_atoi(av[5]);
	else
		g_data->number_of_time_each_philosophers_must_eat = 2147483647;
	if (init_game2() == 0)
		return (0);
	init_philosophers();
	return (1);
}

int				init_game2(void)
{
	if (g_data->number_of_time_each_philosophers_must_eat <= 0)
	{
		write(1, "bad arguments\n", 14);
		return (0);
	}
	if (!(g_data->philo = malloc(sizeof(t_philo) * g_data->nb)))
		return (0);
	g_data->fork = ft_sem(FORK, g_data->nb);
	g_data->msg = ft_sem(MSG, 1);
	g_data->end = ft_sem(END, 0);
	g_data->philo_dead = 0;
	g_data->philo_ok = 0;
	g_data->last_msg = 0;
	return (1);
}

int				start_game(void)
{
	int			i;
	pthread_t	tid;

	i = 0;
	g_data->time_start = get_time();
	while (i < g_data->nb)
	{
		if (pthread_create(&tid, NULL, &ft_philo, &g_data->philo[i]))
			return (0);
		pthread_detach(tid);
		g_data->philo[i].last_eat = g_data->time_start;
		i++;
		usleep(100);
	}
	return (1);
}

int				end_game(void)
{
	int		time;

	if (g_data->philo_dead)
	{
		sem_wait(g_data->msg);
		g_data->last_msg = 1;
		time = get_time() - g_data->time_start;
		ft_putnbr_fd(time, 1);
		write(1, "\t", 1);
		ft_putnbr_fd(g_data->philo_dead, 1);
		write(1, " died\n", 6);
		return (1);
	}
	else if (g_data->philo_ok >= g_data->nb)
	{
		sem_wait(g_data->msg);
		g_data->last_msg = 1;
		write(1, "Everybody has eaten enough times.\n", 34);
		return (1);
	}
	return (0);
}
