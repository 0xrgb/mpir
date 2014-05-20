﻿/*
Copyright 2014 Alex Dyachenko

This file is part of the MPIR Library.

The MPIR Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation; either version 3 of the License, or (at
your option) any later version.

The MPIR Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the MPIR Library.  If not, see http://www.gnu.org/licenses/.  
*/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MPIR.Tests.HugeRationalTests
{
    [TestClass]
    public class ExpressionTests
    {
        [TestMethod]
        public void TestAllExpressions()
        {
            var baseExpr = typeof(RationalExpression);
            var allExpressions =
                baseExpr.Assembly.GetTypes()
                .Where(x => baseExpr.IsAssignableFrom(x) && !x.IsAbstract)
                .ToList();

            using (var a = new HugeRational(-9, 1))
            using (var b = new HugeRational(4, 1))
            using (var r = MpirRandom.Default())
            {
                var expr = a + (-a * 2) * 3 * (a.Abs() * -2 + -64 + a * a) + 116UL + a;
                VerifyPartialResult(r, expr, 44);
                expr = expr + a * 5 + (a+b) * (b + 1) * (b + -3) * b + (b * -a) - (b * 25UL) - a + (b << 3) - ((a*b) << 1);
                VerifyPartialResult(r, expr, -52);
                expr = expr - 2 - 3UL + (b - (a << 1)) + (b * b - 15UL) * (b - a) * (a - 11) * (b - 3UL) - (-340 - a) + (20UL - b);
                VerifyPartialResult(r, expr, 52);
                expr = expr + (-7 - 2 * a) + (28UL - 4 * b) + -(a + b * 2) + (3 * a).Abs();
                VerifyPartialResult(r, expr, 103);
                expr = expr / a + expr / (3 * b) - a / b - b / (a + 10); //to do: adjust result
                VerifyPartialResult(r, expr, -20);
                expr = expr + (b >> 1);
                VerifyPartialResult(r, expr, -18);

                MarkExpressionsUsed(allExpressions, expr);
            }

            Assert.AreEqual(0, allExpressions.Count, "Expression types not exercised: " + string.Join("",
                allExpressions.Select(x => Environment.NewLine + x.Name).OrderBy(x => x)));
        }

        private void VerifyPartialResult(MpirRandom rnd, RationalExpression expr, long expected)
        {
            rnd.Seed(123);

            using (var r = new HugeRational())
            {
                r.Value = expr;
                Assert.AreEqual(expected.ToString(), r.ToString());
            }
        }

        private void MarkExpressionsUsed(List<Type> allExpressions, RationalExpression expr)
        {
            var type = expr.GetType();
            allExpressions.Remove(type);
            
            var children = type.GetFields(BindingFlags.NonPublic | BindingFlags.Instance)
                .Where(x => typeof(RationalExpression).IsAssignableFrom(x.FieldType))
                .Select(x => (RationalExpression)x.GetValue(expr))
                .Where(x => x != null)
                .ToList();

            foreach (var childExpr in children)
                MarkExpressionsUsed(allExpressions, childExpr);
        }
    }
}
